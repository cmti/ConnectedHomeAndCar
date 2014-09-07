#include "mbed.h"
#include "M2XStreamClient.h"
#include "MTSSerial.h"
#include "Wifi.h"
#include "include_me.h"
#include "math.h"

/* This example shows how to do a basic connectivity test to AT&T M2X Cloud 
 * using the MTS Wifi shield board. You will need to change the network
 * SSID and security key. You may need to chage the security type.
 */

using namespace mts;

const char key[] = "your M2X API key";    // Replace with your M2X API key
const char feed[] = "your blueprint Feed ID";   // Replace with your blueprint Feed ID
const char stream[] = "your stream name"; // Replace with your stream name  

DigitalOut buzzer(D5); 
AnalogIn humiditySensor(A2);    

void connect()
{
    //Set the network parameters
    std::string ssid =  "your SSID name"; 
    std::string securityKey = "your SSID key"; 

    Wifi::SecurityType securityType = Wifi::WPA2;
    
    //Setup serial interface to WiFi module
    MTSSerial* serial = new MTSSerial(D8, D2, 256, 4096);   
    serial->baud(9600);

    Transport::setTransport(Transport::WIFI);
    
    //Setup Wifi class
    Wifi* wifi = Wifi::getInstance();
    printf("Init: %s\n\r", wifi->init(serial) ? "SUCCESS" : "FAILURE");

    //Setup and check connection
    printf("Set Network: %s\n\r", getCodeNames(wifi->setNetwork(ssid, securityType, securityKey)).c_str());
    printf("Set DHCP: %s\n\r", getCodeNames(wifi->setDeviceIP("DHCP")).c_str());
    while (! wifi->connect()) {
        printf("Connect: Failure\r\n");
        wait(1);
    }
    printf("Connect: Success\r\n");
    printf("Is Connected: %s\n\r", wifi->isConnected() ? "True" : "False");
    
    printf("Ping Server: %s\n\r", wifi->ping("8.8.8.8") ? "Success" : "Failed");
    wait(1);
}

int main()
{
    int response;
    int a;
        
    //Wait for wifi module to boot up
    for (int i = 10; i >= 0; i = i - 2) {
        wait(2);
        printf("Waiting %d seconds...\n\r", i);
    }

    connect();

    // Initialize the M2X client
    Client client;    
    M2XStreamClient m2xClient(&client, key);

    int on = 1, off = 0, failed = 0;
    float h = 0.0f;
    while(1)    
    {           
        if(failed == 0)
        {
            a = humiditySensor.read_u16();
            h = a/4096.0;
            printf("Current reading from moisture sensor: %2.2f, raw reading: 0x%X = %d", h, a, a);  
        }               
        if(h > 0.8) {
            printf("Relative humidity jumped! Alarm! Beeping!\n");
            buzzer = on;
            wait(5);
            buzzer = off;
        } 
        response = m2xClient.post(feed, stream, h);
        printf("Post response code: %d\r\n", response);
        if(response == -1)
        {
            failed = 1;       
            connect();
        }
        else
        {
            failed = 0;
        }
        delay(2000);        
    }           
        
}

