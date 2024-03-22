#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/net/openthread.h>
#include <openthread/thread.h>
#include <openthread/coap.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS   1000

//Button definition
#define BUTTON0_NODE	DT_NODELABEL(button0)
#define BUTTON1_NODE	DT_NODELABEL(button1)
#define BUTTON2_NODE	DT_NODELABEL(button2)
#define BUTTON3_NODE	DT_NODELABEL(button3)

static const struct gpio_dt_spec button0_spec = GPIO_DT_SPEC_GET(BUTTON0_NODE, gpios);
static const struct gpio_dt_spec button1_spec = GPIO_DT_SPEC_GET(BUTTON1_NODE, gpios);
static const struct gpio_dt_spec button2_spec = GPIO_DT_SPEC_GET(BUTTON2_NODE, gpios);
static const struct gpio_dt_spec button3_spec = GPIO_DT_SPEC_GET(BUTTON3_NODE, gpios);

static struct gpio_callback button0_cb;
static struct gpio_callback button1_cb;
static struct gpio_callback button2_cb;
static struct gpio_callback button3_cb;


static void coap_send_data_request(void);
static void coap_send_data_response_cb(void * p_context, otMessage * p_message, const otMessageInfo * p_message_info, otError result);

static void coap_send_data_response_cb(void * p_context, otMessage * p_message, const otMessageInfo * p_message_info, otError result){
	if(result == OT_ERROR_NONE){
		printk("Delivery confirmed\n");
	}else{
		printk("Delivery not confirmed: %d\n", result);
	}
}

static void send_regular_message(void){
	otError error = OT_ERROR_NONE;
	otMessage * myMessage;
	otMessageInfo myMessageInfo;
	otInstance * myInstance = openthread_get_default_instance();

	const otMeshLocalPrefix *ml_prefix = otThreadGetMeshLocalPrefix(myInstance);
	uint8_t serverInterfaceID[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01};

	// Defining text to be sent
	const char * text = "NormalMessage";

	// Building message
	myMessage = otCoapNewMessage(myInstance, NULL);

	// Appending URI
	error = otCoapMessageAppendUriPathOptions(myMessage, "storedata");

	// Initialising message with CoAP type and code
	otCoapMessageInit(myMessage, OT_COAP_TYPE_ACKNOWLEDGMENT, OT_COAP_CODE_PUT);

	// Appending Formatting option
	error = otCoapMessageAppendContentFormatOption(myMessage, OT_COAP_OPTION_CONTENT_FORMAT_JSON);
	
	// Setting payload marker myMessage
	error = otCoapMessageSetPayloadMarker(myMessage);

	// Error checking
	if(error != OT_ERROR_NONE){
		printk("Error recieved from initialising message: ", error);
	}	

	// Appending string message bytes to the message
	error = otMessageAppend(myMessage, text, strlen(text));

	// Setting Message info data
	memset(&myMessageInfo, 0, sizeof(myMessageInfo));
	memcpy(&myMessageInfo.mPeerAddr.mFields.m8[0], ml_prefix, 8);
	memcpy(&myMessageInfo.mPeerAddr.mFields.m8[8], serverInterfaceID, 8);
	myMessageInfo.mPeerPort = OT_DEFAULT_COAP_PORT;

	// Sending Coap Request
	error = otCoapSendRequest(myInstance, myMessage, &myMessageInfo, NULL, NULL);
	
	// Error checking
	if(error != OT_ERROR_NONE){
		printk("Failed to send coap request: %d\n", error);
		otMessageFree(myMessage);
	}else{
		printk("coap data sent.\n");
	}
}

static void send_blank_packet(void){
	//removed
}

static void coap_send_data_request(void){
	otError error = OT_ERROR_NONE;
	otMessage * myMessage;
	otMessageInfo myMessageInfo;
	otInstance * myInstance = openthread_get_default_instance();

	const otMeshLocalPrefix *ml_prefix = otThreadGetMeshLocalPrefix(myInstance);
	uint8_t serverInterfaceID[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01};

	const char * text = "ThisIsMyMessage";

	do{

		// Building message
		myMessage = otCoapNewMessage(myInstance, NULL);

		// Appending URI
		error = otCoapMessageAppendUriPathOptions(myMessage, "storedata");

		// Initialising message with CoAP type and code
		otCoapMessageInit(myMessage, OT_COAP_TYPE_ACKNOWLEDGMENT, OT_COAP_CODE_PUT);

		// Appending Formatting option
		error = otCoapMessageAppendContentFormatOption(myMessage, OT_COAP_OPTION_CONTENT_FORMAT_JSON);
		
		// Setting payload marker myMessage
		error = otCoapMessageSetPayloadMarker(myMessage);

		// Error checking
		if(error != OT_ERROR_NONE){
			printk("Error recieved from initialising message: ", error);
		}	

		// Appending string message bytes to the message
		error = otMessageAppend(myMessage, text, strlen(text));

		// Setting Message info data
		memset(&myMessageInfo, 0, sizeof(myMessageInfo));
		memcpy(&myMessageInfo.mPeerAddr.mFields.m8[0], ml_prefix, 8);
		memcpy(&myMessageInfo.mPeerAddr.mFields.m8[8], serverInterfaceID, 8);
		myMessageInfo.mPeerPort = OT_DEFAULT_COAP_PORT;

		////// Fuzzing section //////
		
		// Getting length of message 
		uint16_t len = otMessageGetLength(myMessage);
		uint16_t offset = 15;
		uint16_t messageOffset = otMessageGetOffset(myMessage);
		const uint8_t overwrite_buffer[] = { 0x00 };
		uint16_t read_bytes;
		uint16_t sandwich_bytes;
		uint8_t two_buffer[] = { 0x00, 0x00 };

		// Sections after bitwise shiting (assuming packet data is specifc to this scenario)
		//uint8_t section1_shifted = { 0x00, 0x00, 0x00, 0x00, 0x00, 0xB9, 0x73};
		//uint8_t seciton2_shifted = { 0x00, 0x6F, 0X72, 0X65, 0X64, 0X61, 0X74, 0X61};
		//uint8_t section3_shifted = { 0x00, 0x32 };
		//uint8_t section4_shifted = { 0x00, 0X54, 0X68, 0X69, 0X73, 0X49, 0X73, 0X4D, 0X79, 0X4D, 0X65, 0X73, 0X73, 0X61, 0X67};


		// Reading the message
		uint8_t* message_bytes = malloc(len);
		uint8_t* sandwich_buffer = malloc(sizeof(two_buffer));
		read_bytes = otMessageRead(myMessage, 0, message_bytes, len);
		sandwich_bytes = otMessageRead(myMessage, 15, sandwich_buffer, 2);
		

		// Handling random bit flips
		srand(time(NULL));
		uint8_t* message_buffer = malloc(sizeof(7));
		uint16_t read_start_bytes = otMessageRead(myMessage, 0, message_buffer, len);
		int length = sizeof(message_buffer);

		// Applying bit flip and printing results
		for (int i = 0; i < len; i++){
			if (rand() % 2 ){
				// xor mask to flip bits
				message_buffer[i] ^= 1U << i;
				printk("%02X:", message_bytes[i]);
			}
		}
		printk(" (%u)\n", read_start_bytes);


		//Shifting buffers
		// for (int i = 0; i < length; i++){
		// 	message_buffer[i] = message_buffer[i] >> 1;
		// 	printk("%02X\n", sandwich_buffer[i]);
		// }
		// sandwich_buffer[0] = sandwich_buffer[0] >> 1;
		// sandwich_buffer[1] = sandwich_buffer[1] >> 1;
		// printk("%02X\n", sandwich_buffer[0]);
		// printk("%02X\n", sandwich_buffer[1]);


		//Writing to message
		//uint16_t sandwich_written = otMessageWrite(myMessage, 0, message_bytes, 7);	// Even tho the whole message is fuzzed we are only overwriting 7 hex pairs.	
		//uint16_t sandwich_written = otMessageWrite(myMessage, 7, message_bytes, 8);	 // Overwriting Section 2 // Annoying when testing if you forget this is the URI info, end up spamming the button and wondering why its not working.
		//uint16_t sandwich_written = otMessageWrite(myMessage, 15, message_bytes, 2); // Overwriting Section3
		uint16_t sandwich_written = otMessageWrite(myMessage, 17, message_bytes, 15);	// Overwriting Section4
		printk("Message Before: ");
		for (uint16_t i = 0; i != read_bytes; ++i)
		{
			printk("%02X:", message_bytes[i]);
		}
		printk(" (%u)\n", read_bytes);

		//otMessageWrite(myMessage, 1, code_buffer, 1);

		//uint16_t written = otMessageWrite(myMessage, offset, overwrite_buffer, 2);

		//printk("Overwritten %u bytes at %u\n", sandwich_written, offset);

		// otMessageWrite(myMessage, 4, overwrite_buffer, 3);
		// otMessageRead
		read_bytes = otMessageRead(myMessage, 0, message_bytes, len);
		printk("Message After : ");
		for (uint16_t i = 0; i != read_bytes; ++i)
		{
			printk("%02X:", message_bytes[i]);
		}
		printk(" (%u)\n", read_bytes);

		free(sandwich_buffer);

		error = otCoapSendRequest(myInstance, myMessage, &myMessageInfo, coap_send_data_response_cb, NULL);

		read_bytes = otMessageRead(myMessage, 0, message_bytes, len);
		// printk("Message After Being Sent: ");
		// for (uint16_t i = 0; i != read_bytes; ++i)
		// {
		// 	printk("%02X:", message_bytes[i]);
		// }
		// printk("\n");
		printk("Message Sent\n");
		free(message_bytes);

	} while(false);

	if(error != OT_ERROR_NONE){
		printk("Failed to send coap request: %d\n", error);
		otMessageFree(myMessage);
	}
}	

void button0_pressed_callback(const struct device *gpiob, struct gpio_callback *cb, gpio_port_pins_t pins){
	send_regular_message();
}

void button1_pressed_callback(const struct device *gpiob, struct gpio_callback *cb, gpio_port_pins_t pins){
	coap_send_data_request();
}
void button2_pressed_callback(const struct device *gpiob, struct gpio_callback *cb, gpio_port_pins_t pins){
	send_blank_packet();
}

void button3_pressed_callback(const struct device *gpiob, struct gpio_callback *cb, gpio_port_pins_t pins){
	// coap_send_bad_packet(COAP_METHOD_GET);
}


void ot_coap_init(void){
	otInstance * p_instance = openthread_get_default_instance();
	otError error = otCoapStart(p_instance, OT_DEFAULT_COAP_PORT);
	if(error != OT_ERROR_NONE){
		printk("failed to start coap: %d\n", error);
	}	
}

int main(void)
{	
	ot_coap_init();

	gpio_pin_configure_dt(&button0_spec, GPIO_INPUT);
	gpio_pin_configure_dt(&button1_spec, GPIO_INPUT);
	gpio_pin_configure_dt(&button2_spec, GPIO_INPUT);
	gpio_pin_configure_dt(&button3_spec, GPIO_INPUT);

	gpio_pin_interrupt_configure_dt(&button0_spec,GPIO_INT_EDGE_TO_INACTIVE); // configured interrupt
	gpio_pin_interrupt_configure_dt(&button1_spec,GPIO_INT_EDGE_TO_INACTIVE); 
	gpio_pin_interrupt_configure_dt(&button2_spec,GPIO_INT_EDGE_TO_INACTIVE); 
	gpio_pin_interrupt_configure_dt(&button3_spec,GPIO_INT_EDGE_TO_INACTIVE); 
	
	gpio_init_callback(&button0_cb, button0_pressed_callback, BIT(button0_spec.pin));  /* Initialize the struct gpio_callback button_cb_data by passing this variable,along with the callback function and the bit mask for the GPIO pin button.pin to gpio_init_callback().*/
	gpio_init_callback(&button1_cb, button1_pressed_callback, BIT(button1_spec.pin));
	gpio_init_callback(&button2_cb, button2_pressed_callback, BIT(button2_spec.pin));  
	gpio_init_callback(&button3_cb, button3_pressed_callback, BIT(button3_spec.pin));

	gpio_add_callback(button0_spec.port, &button0_cb); // add the callback func
	gpio_add_callback(button1_spec.port, &button1_cb);
	gpio_add_callback(button2_spec.port, &button2_cb); 
	gpio_add_callback(button3_spec.port, &button3_cb);


	while (1) {
		k_msleep(SLEEP_TIME_MS);
	}
}
