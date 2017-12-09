#include <stdio.h>
#include <unistd.h>
#include <cstdint>
#include <ws2811.h>
#include <thread>
#include <mutex>

#define TARGET_FREQ             WS2811_TARGET_FREQ
#define GPIO_PIN                12
#define DMA                     5
#define LED_COUNT				32
#define LED_INVERT				false
#define LED_BRIGHTNESS			20
#define STRIP_TYPE				WS2811_STRIP_GRB



namespace alexaClientSDK {
namespace led {

enum LEDState {
	IDLE,
	LISTENING,
	THINKING,
	SPEAKING,
	FINISHED,
	VOLUME,
	ERROR,
	ALARM,
	TIMER,
	STARTUP,
	NOTIFICATION
};

class LEDControl{
public:
	LEDControl();
	~LEDControl();
//Animations
	void DOA(int doa);
	void volume(int volume);
	void thinking();
	void startup();
	void speech();
	void finished();
	void error();
	void alarm();
	void timer();
	void micMute();
	void micUnmute();
//	void wifi();

//Variables
	//Tells the animation to stop and release the mutex
	bool m_stop;
	//Current animation
	LEDState m_state = LEDState::STARTUP;
	//For serialization, public since TLED will be changing the states
	std::mutex m_mutex;

private:	
	//Returns color as uint
	ws2811_led_t color(unsigned char r, unsigned char g, unsigned char b, unsigned char w = 0);
	//Fades color by percentage
	ws2811_led_t fadeColor(ws2811_led_t color, unsigned char percentage); 
	//Blends two color with color2 by percentage amount
	ws2811_led_t blendColor(ws2811_led_t color, ws2811_led_t color2, unsigned char percentage);
	//Pulse animation
	void pulse(ws2811_led_t color, uint32_t sleepMicros = 50000, unsigned char iterations = 10);
	//Sets the color of a pixel
	void setPixelColor(ws2811_led_t color, int pixel);
	//Sets the color of all pixels
	void setColor(ws2811_led_t color);
	//Sets all pixels to 0 (off)
	void clearColor();
	//Renders the changes made since last render() call
	void render();
	int getDirection();
	//Render loop run by ledThread
	void ledLoop();
	//Ends the render loop
	bool m_kill = 0;
	//Might be used for special cases?
	LEDState m_oldState;
	//Thread that runs the render loop
	std::thread *m_ledThread; 
	//LED init struct
	ws2811_t m_ledstring =
	{
		0,
		NULL,
		NULL,
	    .freq = TARGET_FREQ,
	    .dmanum = DMA,
	    .channel =
	    {
	        [0] =
	        {
	            .gpionum = GPIO_PIN,
	            .invert = LED_INVERT,
	            .count = LED_COUNT,
	            .strip_type = STRIP_TYPE,
	            NULL,
	            .brightness = LED_BRIGHTNESS,
	            0,
	            0,
	            0,
	            0,
	            0
	        },
	    },
	};
	//Last direction of speaker
	int m_old_pixel = 0;
	//Colors used
	ws2811_led_t ledcolors[5] =
	{
	    0x00db0000,  // red mic off
	    0x00ff8c00,  // orange system error
	    0x00efe823,  // yellow notification
	    0x0028c4cc,  // Cyan    
	    0x001f33c2   // blue
	};
};
}
}