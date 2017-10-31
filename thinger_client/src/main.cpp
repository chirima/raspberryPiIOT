// The MIT License (MIT)
//
// Copyright (c) 2015 THINGER LTD
// Author: alvarolb@gmail.com (Alvaro Luis Bustamante)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include "thinger/thinger.h"
#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define USER_ID             "Abhishek66"
#define DEVICE_ID           "raspberrypi3"
#define DEVICE_CREDENTIAL   "abhiPI"

#define MAXTIMINGS          85
#define DHTPIN              7

int main(int argc, char *argv[])
{
    thinger_device thing(USER_ID, DEVICE_ID, DEVICE_CREDENTIAL);
    if (wiringPiSetup() == -1)
    {
        exit(1);
    }
    // define thing resources here. i.e, this is a sum example
    /*thing["sum"] = [](pson& in, pson& out){
        out["result"] = (int) in["value1"] + (int) in["value2"];
    };*/
    while(1)
    {
        thing["Readings"] >> [](pson & out)
        {
            int dht11_dat[5] = {0, 0, 0, 0, 0};
            uint8_t laststate = HIGH;
            uint8_t counter = 0;
            uint8_t j = 0, i;
            dht11_dat[0] = dht11_dat[1] = dht11_dat[2] = dht11_dat[3], dht11_dat[4] = 0;
            
            pinMode(DHTPIN, OUTPUT);
            digitalWrite(DHTPIN, LOW);
            delay(18);
            digitalWrite(DHTPIN, HIGH);
            delayMicroseconds(40);
            pinMode(DHTPIN, INPUT);
            for(i=0; i < MAXTIMINGS; i++)
            {
                counter = 0;
                while(digitalRead(DHTPIN) == laststate)
                {
                    counter++;
                    delayMicroseconds(1);
                    if(counter == 255)
                    {
                        break;
                    }
                }
                laststate = digitalRead(DHTPIN);
                if(counter == 255)
                {
                    break;
                }
                if ((i>=4) && (i%2 == 0))
                {
                    dht11_dat[j/8] = dht11_dat[j/8] << 1;
                    if (counter > 16)
                    {
                        dht11_dat[j/8] = dht11_dat[j/8] | 1;
                        j++;
                    }
                }
            }
            if( (j >= 40) && (dht11_dat[4] == ((dht11_dat[0] + dht11_dat[1] + dht11_dat[2] + dht11_dat[3]) & 0xFF)))
            {
                out["Humidity"] = dht11_dat[0];
                out["Temperature [C]"] = dht11_dat[2]; 
            }
            else
            {
                out["Device"] = "Raspberry Pi 3 Connected";
                out["Humidity"] = dht11_dat[0];
                out["Temperature [C]"] = dht11_dat[2]; 
                out["result"] = "Temp: Data not good";
            }
        };
        thing.start();
    }
    return 0;
}