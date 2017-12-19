#include "LED/LEDControl.h"

#include "clk.h"
#include "gpio.h"
#include "dma.h"
#include "pwm.h"
#include "version.h"

namespace alexaClientSDK {
namespace led {

LEDControl::LEDControl() {
    ws2811_return_t ret;
    if ((ret = ws2811_init(&m_ledstring)) != WS2811_SUCCESS)
    {
        fprintf(stderr, "ws2811_init failed: %s\n", ws2811_get_return_t_str(ret));
    }

    m_ledThread = new std::thread(&alexaClientSDK::led::LEDControl::ledLoop, this);
}

LEDControl::~LEDControl() {
	m_kill = true;
	m_stop = true;

	m_ledThread->join();
	delete m_ledThread;

	setColor(0);
	render();

    ws2811_fini(&m_ledstring);
}

void LEDControl::DOA(int doa) {
	setColor(0);
	if (doa < 360)
	{
		int pixel = doa * LED_COUNT / 360;
		pixel = (50 - pixel) % LED_COUNT;
		if (doa % (360 / LED_COUNT) > (360 / LED_COUNT / 2))
			pixel = (pixel + 1) % LED_COUNT;

		//fprintf(stdout, "%d\n",pixel);

		m_old_pixel = pixel;
	}

	setPixelColor(ledcolors[3], m_old_pixel);
	setPixelColor(ledcolors[3], (m_old_pixel + 1) % LED_COUNT);
	setPixelColor(ledcolors[3], (m_old_pixel - 1 + LED_COUNT) % LED_COUNT);

	render();
}

void LEDControl::volume(int volume) {
	uint32_t micros = 64000;
	int ledc = LED_COUNT * volume / 100;

	clearColor();

	for (int i = 0; i < ledc; i++)
	{
		setPixelColor(ledcolors[3], i);
		render();
		usleep(micros);
		micros -= 2000;
	}

	usleep(2000000);

	for (int i = 1; i <= 10; i++)
	{
		for (int j = 0; j < ledc; j++)
			setPixelColor(fadeColor(ledcolors[3], i*10), j);
		render();
		usleep(30000);
	}
}

void LEDControl::mute() {
	pulse(ledcolors[3], 40000, 10, LED_COUNT/10, false);
	pulse(ledcolors[3], 40000, 10, LED_COUNT/10, false);
}


void LEDControl::thinking() {
	for (int i = 0; i<8; i++)
	{
		for (int j = 0; j< LED_COUNT; j++)
		{
			if(j % 8 < 4)
				setPixelColor(ledcolors[3], (j + i) % LED_COUNT);
			else
				setPixelColor(ledcolors[4], (j + i) % LED_COUNT);
		}
		render();
		if (m_stop)
			return;
		usleep(80000);
	}
	clearColor();
	render();
}


void LEDControl::startup() {
	for(int i = 0; i < LED_COUNT; i++)
	{
		setPixelColor(ledcolors[4], i);
		for(int j = 1; j < 5; j++)
		{
			setPixelColor(ledcolors[3],(i+j)%LED_COUNT);
		}
		render();
		if (m_stop)
			return;
		usleep(1000000/LED_COUNT);
	}
	//clearColor();
	render();
}

//void LEDControl::wifi()

void LEDControl::speech() {
	for (int i = 0; i <= 10; i++)
	{
		setColor(blendColor(ledcolors[3], ledcolors[4], i*10));
		render();
		if (m_stop)
			return;
		usleep(50000);
	}
	for (int i = 10; i >= 0; i--)
	{
		setColor(blendColor(ledcolors[3], ledcolors[4], i*10));
		render();
		if (m_stop)
			return;
		usleep(50000);
	}
	clearColor(); //maybe?
	render();
}

void LEDControl::finished() {
	int micros = 64000;
	//point in direction of speaker
	setColor(ledcolors[4]);
	
	setPixelColor(ledcolors[3], m_old_pixel);
	setPixelColor(ledcolors[3], (m_old_pixel + 1) % LED_COUNT);
	setPixelColor(ledcolors[3], (m_old_pixel - 1 + LED_COUNT) % LED_COUNT);

	render();
	if (m_stop)
		return;
	usleep(100000);

	//setPixelColor(0, m_old_pixel);
	for(int i = 0; i < (LED_COUNT)/2; i++) {
		setPixelColor(0, (m_old_pixel + i) % LED_COUNT);
		setPixelColor(0, (m_old_pixel - i + LED_COUNT) % LED_COUNT);

		setPixelColor(ledcolors[3], (m_old_pixel + i + 1) % LED_COUNT);
		setPixelColor(ledcolors[3], (m_old_pixel - i - 1 + LED_COUNT) % LED_COUNT);

		render();
		if (m_stop)
			return;
		usleep(micros);
	}
	clearColor();
	render();
}

void LEDControl::error() {
	pulse(ledcolors[1]);
}

void LEDControl::alarm() {
	int micros = 50000; //83333;
	//TODO: cyan fill, blue edges black center pulse different places
	setColor(ledcolors[3]);
	int l = 0, half = LED_COUNT/2;
	for(int i = 0; i < 4; i++)
	{
		if(i == 3)
			l = LED_COUNT + 4;
		else
			l = LED_COUNT + i * 4;
		setPixelColor(ledcolors[4], l % LED_COUNT );
		setPixelColor(ledcolors[4], (l + half) % LED_COUNT );
		render();
		if (m_stop)
			return;
		usleep(micros);
		for(int j = 0; j < 4; j++)
		{
			setPixelColor(0, (l + j) % LED_COUNT);
			setPixelColor(0, (l - j) % LED_COUNT);
			setPixelColor(0, (l + j + half) % LED_COUNT);
			setPixelColor(0, (l - j + half) % LED_COUNT);

			setPixelColor(ledcolors[4], (l + j + 1) % LED_COUNT);
			setPixelColor(ledcolors[4], (l - j - 1) % LED_COUNT);
			setPixelColor(ledcolors[4], (l + j + 1 + half) % LED_COUNT);
			setPixelColor(ledcolors[4], (l - j - 1 + half) % LED_COUNT);

			render();
			usleep(micros);
		}
		for(int j = 3; j >= 0; j--)
		{
			setPixelColor(ledcolors[4], (l + j) % LED_COUNT);
			setPixelColor(ledcolors[4], (l - j) % LED_COUNT);
			setPixelColor(ledcolors[4], (l + j + half) % LED_COUNT);
			setPixelColor(ledcolors[4], (l - j + half) % LED_COUNT);

			setPixelColor(ledcolors[3], (l + j + 1) % LED_COUNT);
			setPixelColor(ledcolors[3], (l - j - 1) % LED_COUNT);
			setPixelColor(ledcolors[3], (l + j + 1 + half) % LED_COUNT);
			setPixelColor(ledcolors[3], (l - j - 1 + half) % LED_COUNT);

			render();
			if (m_stop)
				return;
			usleep(micros);
		}
		setColor(ledcolors[3]);
		render();
		if (m_stop)
			return;
		usleep(micros);
	}
}

void LEDControl::timer() {
	alarm(); //Same animation
}

void LEDControl::micMute() {
	for(int i = 4; i>=0; i--)
	{
		setColor(fadeColor(ledcolors[0], i*20));
		render();
		if (m_stop)
			return;
		usleep(50000);
	}
}

void LEDControl::micUnmute() {
	//Think this is what it should do, seems to increase red brightness for a millisecond first, maybe a bug?
	setColor(0);
	render();
}

ws2811_led_t LEDControl::color(unsigned char r, unsigned char g, unsigned char b, unsigned char w) {
	return (w << 24 | r << 16 | g << 8 | b);
}

//Fades by percentage amount
ws2811_led_t LEDControl::fadeColor(ws2811_led_t color, unsigned char percentage) {
	if (percentage >= 100 || percentage < 0)
		return 0;

	double ratio = (100.0 - percentage) / 100;

	return 	(unsigned char)(((color >> 24) & 255) * ratio) << 24 | 
			(unsigned char)(((color >> 16) & 255) * ratio) << 16 | 
			(unsigned char)(((color >> 8) & 255) * ratio) << 8 |
			(unsigned char)((color & 255) * ratio);
}

ws2811_led_t LEDControl::blendColor(ws2811_led_t color, ws2811_led_t color2, unsigned char percentage) {
	if (percentage > 100 || percentage < 0)
		return 0;

	double ratio = (double)percentage / 100;

	return 	(unsigned char)(((color >> 24) & 255) * (1.0 - ratio) + ((color2 >> 24) & 255) * ratio) << 24 | 
			(unsigned char)(((color >> 16) & 255) * (1.0 - ratio) + ((color2 >> 16) & 255) * ratio) << 16 | 
			(unsigned char)(((color >> 8) & 255) * (1.0 - ratio) + ((color2 >> 8) & 255) * ratio) << 8 |
			(unsigned char)((color & 255) * (1.0 - ratio) + (color2 & 255) * ratio);
}


void LEDControl::pulse(ws2811_led_t col, uint32_t sleepMicros, unsigned char iterations, int count, bool doStop) {
	int j = 100/iterations;
	for (int i = iterations; i >= 0; i--) 
	{
		for(int k = 0; k < count; k++)
			setPixelColor(fadeColor(col, i * j), k);
		render();
		if (doStop && m_stop)
			return;
		usleep(sleepMicros);
	}
	//usleep(sleepMicros*2);
	for (int i = 0; i<=iterations; i++) 
	{
		for(int k = 0; k < count; k++)
			setPixelColor(fadeColor(col, i * j), k);
		render();
		if (doStop && m_stop)
			return;
		usleep(sleepMicros);
	}
}

void LEDControl::setPixelColor(ws2811_led_t color, int pixel) {
	if (pixel >= 0 && pixel < LED_COUNT)
		m_ledstring.channel[0].leds[pixel] = color;
}


void LEDControl::clearColor() {
	for (int i = 0; i<LED_COUNT; i++)
		m_ledstring.channel[0].leds[i] = 0;
}

void LEDControl::setColor(ws2811_led_t color) {
	for (int i = 0; i<LED_COUNT; i++)
		m_ledstring.channel[0].leds[i] = color;
}

void LEDControl::render() {
    ws2811_return_t ret;

    if ((ret = ws2811_render(&m_ledstring)) != WS2811_SUCCESS)
    {
        fprintf(stderr, "ws2811_render failed: %s\n", ws2811_get_return_t_str(ret));
//        return true;
    }
//    return false;
}
int LEDControl::getDirection() {
    char buffer[128];
    FILE* pipe = popen("./host_demo.exe -u --4micDOA 2 2>/dev/null", "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
	try {
        fgets(buffer, 128, pipe);
    } catch (...) {
    	pclose(pipe);
    	//TODO: Use proper log
    	fprintf(stderr, "getDirection failed\n");
    	return 360;
    }
    pclose(pipe);
    return atoi(buffer);
}
void LEDControl::ledLoop() {
	while(!m_kill)
	{
		//Lock so that the state doesn't change mid loop
		m_mutex.lock();

		switch(m_tempState) {
			case TempState::VOLUME:
				volume(m_volume);
				m_tempState = TempState::NONE;
				break;
			case TempState::MUTED:
				mute();
				m_tempState = TempState::NONE;
				break;
			default:
				break;
		}

		//Run until the state is going to change
		while(!m_stop) {
			switch(m_state) {
				case LEDState::IDLE:
					usleep(5000);
					break;
				case LEDState::LISTENING:
					DOA(getDirection());
					//TODO: Get real value 
					break;
				case LEDState::THINKING:
					thinking();
					break;
				case LEDState::SPEAKING:
					speech();
					break;
				case LEDState::FINISHED:
					finished();
					break;
				case LEDState::ERROR:
					error();
					break;
				case LEDState::ALARM:
					alarm();
					break;
				case LEDState::TIMER:
					timer();
					break;
				case LEDState::STARTUP:
					startup();
					break;
				case LEDState::NOTIFICATION:
					//notification();
					break;
				default:
					continue;
			}
		}
		//Lock got asked for to change the state
		clearColor();
		render();
		m_mutex.unlock();
		m_stop = false;

		usleep(50000);
	}
}
}}