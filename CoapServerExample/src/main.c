#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/net/openthread.h>
#include <openthread/thread.h>
#include <openthread/coap.h>

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS   1000

static void storedata_request_cb(void * p_context, otMessage * p_message, const otMessageInfo * p_message_info);

static void storedata_response_send(otMessage * p_request_message, const otMessageInfo * p_message_info);

#define TEXTBUFFER_SIZE 30
char myText[TEXTBUFFER_SIZE];
uint16_t myText_length = 0;

static otCoapResource m_storedata_resource = {
	.mUriPath = "storedata",
	.mHandler = storedata_request_cb,
	.mContext = NULL,
	.mNext = NULL
};

static void storedata_request_cb(void * p_context, otMessage * p_message, const otMessageInfo * p_message_info){

	otCoapCode messageCode = otCoapMessageGetCode(p_message);
	otCoapType messageType = otCoapMessageGetType(p_message);

	do {
		uint8_t* message_buffer = malloc(otMessageGetLength(p_message));
		uint16_t read_bytes = otMessageRead(p_message, 0, message_buffer, otMessageGetLength(p_message));
		
		// Used for seeing the value of the message recieved
		// printk("Printing read message\n");
		// for (uint16_t i = 0; i != read_bytes; ++i)
		// {
		// 	printk("%02X:", message_buffer[i]);
		// }
		printk("\n");

		//Error checking removed
		if (messageType != OT_COAP_TYPE_CONFIRMABLE){
			printk("Coap type Not confirmable \n");
			// break; 
		}

		// Error checking removed
		if (messageCode != OT_COAP_CODE_PUT){
			printk("Coap type Not PUT\n");
			//break;
		}

		// Decoding incoming message
		myText_length = otMessageRead(p_message, otMessageGetOffset(p_message), myText, TEXTBUFFER_SIZE - 1);
		myText[myText_length]='\0';
		printk("Decoded message: %s\n", myText);

		// Attempts to send a response back to the client
		printk("Attempting response message\n");

		// This is the line which casuses the server to crash
		// Comment out to stop crashing
		storedata_response_send(p_message, p_message_info);
		if(messageType == OT_COAP_TYPE_CONFIRMABLE){
		}		
	} while(false);
}

static void storedata_response_send(otMessage * p_request_message, const otMessageInfo * p_message_info){
	otError error = OT_ERROR_NONE;
	otMessage * p_response;
	otInstance * p_instance = openthread_get_default_instance();

	p_response = otCoapNewMessage(p_instance, NULL);
	if( (p_response = NULL) ){
		printk("Failed to allocate message for coap request\n");
		return;
	}
	
	error = otCoapMessageInitResponse(p_response, p_request_message, OT_COAP_TYPE_ACKNOWLEDGMENT, OT_COAP_CODE_CHANGED);

	// if(error != OT_ERROR_NONE){
	// 	break;
	// }
	error = otCoapSendResponse(p_instance, p_response, p_message_info);
	

	if(error != OT_ERROR_NONE){
		printk("Failed to send store data response: %d\n", error);
		otMessageFree(p_response);
	}
}

// Initialising coap server
void coap_init(void){
	otError error;
	otInstance * p_instance = openthread_get_default_instance();
	m_storedata_resource.mContext = p_instance;

	
	otCoapStart(p_instance, OT_DEFAULT_COAP_PORT);
	otCoapAddResource(p_instance, &m_storedata_resource);


	if(error != OT_ERROR_NONE){
		printk("coapinit error: %d\n", error);
	}
}

// Adding IPv6 address
// Needs to match the CoAP client
void addIPv6Address(void){
	otInstance *myInstance = openthread_get_default_instance();
	otNetifAddress aAddress;
	const otMeshLocalPrefix *ml_prefix = otThreadGetMeshLocalPrefix(myInstance);
	uint8_t interfaceID[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01};

	memcpy(&aAddress.mAddress.mFields.m8[0], ml_prefix, 8);
	memcpy(&aAddress.mAddress.mFields.m8[8], interfaceID, 8);

	otError error = otIp6AddUnicastAddress(myInstance, &aAddress);

	if(error != OT_ERROR_NONE){
		printk("addipaddress error : %d\n", error);
	}
}



int main(void)
{
	
	addIPv6Address();
	coap_init();
	printk("Coap server Initialised\n");

	while (1) {
		k_msleep(SLEEP_TIME_MS);
	}
}
