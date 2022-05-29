#include "PS2Keyboard.h"
#include "debug.h"

static uint8_t DataPin;

class IntHandler {
	static const int DELAY_FILTER = 50;   // If the PS2 keyboard hasn't clocked in a bit for this many milliseconds, then it's definitely waiting to start a new key when the user presses one.
	static const int BUFFER_SIZE = 45;
	volatile uint8_t buffer[BUFFER_SIZE];
	volatile uint8_t head, tail;

	enum IntState {
		STARTBIT,
		DATABITS,
		PARITY,
		STOP
	};

	IntState state;

	uint8_t data_pin;
	uint8_t startbit = 0;
	uint8_t paritybit = 0;
	uint8_t stopbit = 0;
	uint8_t onebits = 0;

	uint8_t bitcount=0;
	uint8_t incoming=0;
	uint32_t prev_ms=0;

	void reset() {
		bitcount = 0;
		incoming = 0;
		startbit = 0;
		stopbit = 0;
		paritybit = 0;
		onebits = 0;
		state = STARTBIT;
	}

	void handle_startbit(uint8_t val) {
    dprint("START\n");
		startbit = val;
		if(startbit == 0){
			state = DATABITS;
		}
		else {
			dprint("START BIT was invalid\n");
		}
	}

	void handle_databit(uint8_t val) {
//    dprint("DATA\n");
		incoming |= (val << bitcount);
		bitcount++;
		if(val)
			onebits++;

//    dprintf("bitcount: %d\n", bitcount);
    
		if(bitcount == 8) {
			state = PARITY;
		}
     
	}

	void handle_parity(uint8_t val) {
    dprint("PARITY\n");

		paritybit = val;
		onebits += paritybit;
		if(onebits % 2){
			state = STOP;
		}
		else {
			dprintf("INVALID PARITY: %d and data %d\n", onebits, incoming);
      reset();
		}
	}

	void handle_stop(uint8_t val) {
    dprint("STOP\n");

		stopbit = val;
		if(stopbit == 1) {
			// store the "incoming" value in the circular buffer
			uint8_t i = head + 1;
			if (i >= BUFFER_SIZE) i = 0;
			if (i != tail) {
				buffer[i] = incoming;
				head = i;
			}
		}
		else {
			dprint("STOP BIT was invalid\n");
		}
    reset();
	}

	inline uint8_t read_bit() {
		return digitalRead(data_pin);
	}

	void int_handler(void) {
    uint8_t val = read_bit();
    dprintf("%d ", val);

		uint32_t now_ms;
	
		now_ms = millis();
		if (now_ms - prev_ms > DELAY_FILTER) {
			reset();
		}
		prev_ms = now_ms;
 
		switch(state) {
			case STARTBIT:
				handle_startbit(val);
				break;
			case DATABITS:
				handle_databit(val);
				break;
			case PARITY:
				handle_parity(val);
				break;
			case STOP:
				handle_stop(val);
				break;
		}
	}

public:
	IntHandler() {reset();}
	static void handler(void);

	uint8_t get_scan_code(void)
	{
		uint8_t i = tail;
		if (i == head) 
			return 0;	
		if (++i >= BUFFER_SIZE) 
			i = 0;

		uint8_t c = buffer[i];
		tail = i;
		return c;
	}

	bool available(void) {
		return tail != head;
	}

	void set_data_pin(uint8_t pin) {
			head = 0;
			tail = 0;
			data_pin = pin;
		}

};

IntHandler the_handler;

void IntHandler::handler(void)
{
	the_handler.int_handler();
}



bool PS2Keyboard::available() {
  return the_handler.available();
}

uint8_t PS2Keyboard::read() {
  return the_handler.get_scan_code();
}

PS2Keyboard::PS2Keyboard(uint8_t dataPin, uint8_t irqPin) {
  data_pin = dataPin;
  irq_pin = irqPin;
}

void PS2Keyboard::begin() {

	// initialize the pins
	pinMode(irq_pin, INPUT_PULLUP);
	pinMode(data_pin, INPUT_PULLUP);

	uint8_t irq_num = digitalPinToInterrupt(irq_pin);
	attachInterrupt(irq_num, IntHandler::handler, FALLING);

	the_handler.set_data_pin(data_pin);
}
