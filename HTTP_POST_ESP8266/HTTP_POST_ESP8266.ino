      /*
     * By- Nimit Khandelwal
     * ARDUINO UNO EXAMPLE -ESP8266 HTTP-PUT request
     * This example code is used to interface esp8266 module to arduino uno-r3.
     * Here esp8266 has been setup on station mode to perform HTTP POST request to server.
     */  #include <SoftwareSerial.h>
    #define SSID        "TIGER hostel"
    #define PASS        "ughostel@boys"
    #define DEST_HOST   "172.16.7.159"
    #define DEST_IP     "172.16.7.159"
    #define TIMEOUT     5000 // mS
    #define CONTINUE    false
    #define HALT        true
    #define ECHO_COMMANDS // Un-comment to echo AT+ commands to serial monitor
    
    int pulse=0,sat=8;
    float lan=0,lon=99;
  SoftwareSerial dbgSerial(6, 7);
    // Print error message and loop stop.
    void errorHalt(String msg)
    {
      Serial.println(msg);
      Serial.println("HALT");
      while(true){};
    }
    // Read characters from WiFi module and echo to serial until keyword occurs or timeout.
    boolean echoFind(String keyword)
    {
      byte current_char   = 0;
      byte keyword_length = keyword.length();

      // Fail if the target string has not been sent by deadline.
      long deadline = millis() + TIMEOUT;
      while(millis() < deadline)
      {
        if (dbgSerial.available())
        {
          char ch = dbgSerial.read();
          Serial.print(ch);
          if (ch == keyword[current_char])
            if (++current_char == keyword_length)
            {
              Serial.println();
              return true;
            }
        }
      }
      return false;  // Timed out
    }

    // Read and echo all available module output.
    // (Used when we're indifferent to "OK" vs. "no change" responses or to get around firmware bugs.)
    void echoFlush()
      {while(dbgSerial.available()) Serial.write(dbgSerial.read());}

    // Echo module output until 3 newlines encountered.
    // (Used when we're indifferent to "OK" vs. "no change" responses.)
    void echoSkip()
    {
      echoFind("\n");        // Search for nl at end of command echo
      echoFind("\n");        // Search for 2nd nl at end of response.
      echoFind("\n");        // Search for 3rd nl at end of blank line.
    }

    // Send a command to the module and wait for acknowledgement string
    // (or flush module output if no ack specified).
    // Echoes all data received to the serial monitor.
    boolean echoCommand(String cmd, String ack, boolean halt_on_fail)
    {
      dbgSerial.println(cmd);
      #ifdef ECHO_COMMANDS
        Serial.print("--"); Serial.println(cmd);
      #endif

      // If no ack response specified, skip all available module output.
      if (ack == "")
        echoSkip();
      else
        // Otherwise wait for ack.
        if (!echoFind(ack))          // timed out waiting for ack string 
          if (halt_on_fail)
            errorHalt(cmd+" failed");// Critical failure halt.
          else
            return false;            // Let the caller handle it.
      return true;                   // ack blank or ack found
    }

    // Connect to the specified wireless network.
    boolean connectWiFi()
    {
      String cmd = "AT+CWJAP=\""; cmd += SSID; cmd += "\",\""; cmd += PASS; cmd += "\"";
      if (echoCommand(cmd, "OK", CONTINUE)) // Join Access Point
      {
        Serial.println("Connected to WiFi.");
        return true;
      }
      else
      {
        Serial.println("Connection to WiFi failed.");
        return false;
      }
    }

    // ******** SETUP ********
    void setup()  
    {
      Serial.begin(115200);         // Communication with PC monitor via USB
      dbgSerial.begin(115200);        // Communication with ESP8266 via 5V/3.3V level shifter

      dbgSerial.setTimeout(TIMEOUT);
      Serial.println("ESP8266 Demo");

      Serial.println("Module is ready.");
      //echoCommand("AT+GMR", "OK", CONTINUE);   // Retrieves the firmware ID (version number) of the module. 
      //echoCommand("AT+CWMODE?","OK", CONTINUE);// Get module access mode. 

      // echoCommand("AT+CWLAP", "OK", CONTINUE); // List available access points - DOESN't WORK FOR ME

      echoCommand("AT+CWMODE=1", "", HALT);    // Station mode
      echoCommand("AT+CIPMUX=0", "", HALT);    // Allow multiple connections (we'll only use the first).

      //connect to the wifi
      boolean connection_established = false;
      for(int i=0;i<5;i++)
      {
        if(connectWiFi())
        {
          connection_established = true;
          break;
        }
      }
      if (!connection_established) errorHalt("Connection failed");

      delay(1000);

      //echoCommand("AT+CWSAP=?", "OK", CONTINUE); // Test connection
      echoCommand("AT+CIFSR", "", HALT);         // Echo IP address. (Firmware bug - should return "OK".)
      //echoCommand("AT+CIPMUX=0", "", HALT);      // Set single connection mode
    }

    // ******** LOOP ********
    void loop() 
    {
      // Establish TCP connection
      String cmd = "AT+CIPSTART=\"TCP\",\""; cmd += DEST_IP; cmd += "\",3030";
      if (!echoCommand(cmd, "CONNECTED", CONTINUE)) return;
      delay(1000);

      // Get connection status 
      //if (!echoCommand("AT+CIPSTATUS", "OK", CONTINUE)) 
      //return;

      // Build HTTP request.
      String postData="data={\"pulse\":";
         postData+=String(pulse++);
      postData+=",\"lat\":";
      postData+=String(lan);
      postData+=",\"lon\":";
      postData+=String(lon);
      postData+=",\"sat\":";
      postData+=String(sat);
      postData+="}";
      // Build HTTP request.
      cmd = "POST /";
      cmd +="api/device";
      cmd+=" ";
      cmd += "HTTP/1.0\r\nHost: ";
      cmd += DEST_HOST; 
      cmd += ":3030\r\n";
      //cmd+="User-Agent: Arduino/1.0\r\n";
       cmd+="Content-Length:";
      cmd+=String(postData.length());
      cmd+="\r\n";
      cmd+="Content-Type: application/x-www-form-urlencoded \r\n";
      cmd+=postData;

      // Ready the module to receive raw data
      if (!echoCommand("AT+CIPSEND="+String(cmd.length()), ">", CONTINUE))
      {
        echoCommand("AT+CIPCLOSE", "", CONTINUE);
        Serial.println("Connection timeout.");
        return;
      }

      // Send the raw HTTP request
      echoCommand(cmd, "OK",CONTINUE );  // GET

      // Loop forever echoing data received from destination server.
      
        while (dbgSerial.available())
          Serial.write(dbgSerial.read());

      //errorHalt("ONCE ONLY");
    }
