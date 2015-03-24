/*      	     *
 *  Arduino Switch Web Server     *
 *	    24 March 2015         *
 *        by Alberto Panu         *
 *      alberto[at]panu.it        *
 *       based on work of         *

    copyright Alberto Panu 2015
    
    Ardumando https://github.com/bigjohnson/Ardumando
    
    Your comments and suggestion are welcomed to alberto[at]panu.it
    
    Released under GPL licence V3 see http://www.gnu.org/licenses/gpl.html and file gpl.txt
*/

// Standart libaryes from arduino 1.0.1
#include <pins_arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>

// Need extra Flash library from http://arduiniana.org/libraries/flash/
#include <Flash.h>

// Need extra MyTinyWebServer library included in https://github.com/bigjohnson/ArduPower
#include <MyTinyWebServer.h>
// Or if you wont you can use the original library from https://github.com/ovidiucp/MyTinyWebServer
//#include <TinyWebServer.h>

/****************VALUES YOU CHANGE*************/
// pin 4 is the SPI select pin for the SDcard
#define SD_CS 4

// pin 10 is the SPI select pin for the Ethernet
#define ETHER_CS 10

// Enable serial print debug message
// #define DEBUG 1

// The 4 pin is used by SD_CS and for future development is jumped
static byte ioports[] = { 2, 3, 5, 6, 7, 8, 9 };
#define ioportsnum 7
byte ioportsStatus[ioportsnum];
byte actualread = 0;
unsigned int analogValues[6];
byte actualanalog = 0;

// Don't forget to modify the IP to an available one on your home network
//byte ip[] = { 192, 168, 1, 1 };
//byte gateway[] = { 192, 168, 1, 10 };
//byte subnet[] = { 255, 255, 255, 0 };
byte ip[] = { 10, 63, 0, 73 };
byte gateway[] = { 10, 63, 0, 1 };
byte subnet[] = { 255, 255, 254, 0 };
/*********************************************/
// Substitute with the eth shiel mac address
//static uint8_t mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
static uint8_t mac[] = { 0x00, 0x23, 0x32, 0x95, 0x3D, 0x8F };

boolean index_handler(MyTinyWebServer& web_server);
boolean json_handler(MyTinyWebServer& web_server);
boolean bottone_handler(MyTinyWebServer& web_server);

MyTinyWebServer::PathHandler handlers[] = {
  {"/", MyTinyWebServer::GET, &index_handler },
  {"/json" "*", MyTinyWebServer::GET, &json_handler },
  {"/bottone" "*", MyTinyWebServer::GET, &bottone_handler },
  {NULL},
};

boolean json_handler(MyTinyWebServer& web_server) {
  web_server.send_error_code(200);
  web_server.end_headers();
  
  web_server << F("processData( { ");
    for (byte i=0; i < ioportsnum; i++) {
      web_server << F("\"IO");
      web_server << (int) ioports[i];
      web_server << F("\" : \"");
      //if (digitalRead(ioports[i])) {
        if (ioportsStatus[i]) {
        web_server << F("H");
      } else {
        web_server << F("L");
      }
      web_server << F("\", ");
    }
    for (byte i=0; i < 6; i++){
      web_server << F("\"AD");
      web_server << (int) i;
      web_server << F("\" : \"");
//      web_server << analogRead(i);
       web_server << analogValues[i];
      web_server << F("\"");
      if ( i < 5) {
        web_server << F(", ");
      }
    }
    web_server << F("} );\n"); 
    #ifdef DEBUG
      Serial << F("Sent json\n");
    #endif
  return true;
}

boolean bottone_handler(MyTinyWebServer& web_server) {
  web_server.send_error_code(200);
  web_server.end_headers();
  char porta = web_server.get_path()[8];
  switch (porta) {
      case '2':
        digitalWrite(2, !digitalRead(2));
        break;
      case '3':
        digitalWrite(3, !digitalRead(3));
        break;
      case '5':
        digitalWrite(5, !digitalRead(5));
        break;
      case '6':
        digitalWrite(6, !digitalRead(6));
        break;
      case '7':
        digitalWrite(7, !digitalRead(7));
        break;
      case '8':
        digitalWrite(8, !digitalRead(8));
        break;
      case '9':
        digitalWrite(9, !digitalRead(9));
        break;  
    }
  //web_server << web_server.get_path();
  //web_server << F("processData( { ");
  //web_server << F("\"IO");
 // web_server << porta;
}



boolean index_handler(MyTinyWebServer& web_server) {
  web_server.send_error_code(200);
  web_server.end_headers();
  web_server << F("<html>\n\
  <head>\n\
   <title>Ardumando</title>\n\
    <SCRIPT language=\"JavaScript\">\n\
     <!--hide from old browsers\n\
\n\
     var ciclo = 500 ;\n\
     var tempomassimo = 1500;\n\
     var midanalog=511;\n\
      var intervallo;\n\
      var lastexecutiontime;\n\
      var ajaxisrunning;\n\
    var port_high = \"HIGH\";\n\
    var port_low = \"LOW\";\n\
\n\
      function ajaxObject(url, callbackFunction) {\n\
      	var that=this;\n\
      	this.updating = false;\n\
      	this.abort = function() {\n\
      		if (that.updating) {\n\
      	 that.updating=false;\n\
      	 that.AJAX.abort();\n\
      	 that.AJAX=null;\n\
      		}\n\
      	}\n\
      	this.update = function(passData,postMethod) {\n\
      	if (that.updating) { return false; }\n\
      		that.AJAX = null;\n\
      		if (window.XMLHttpRequest) {\n\
      	 that.AJAX=new XMLHttpRequest();\n\
      		} else {\n\
      	 that.AJAX=new ActiveXObject(\"Microsoft.XMLHTTP\");\n\
      		}\n\
      		if (that.AJAX==null) {\n\
      	 return false;\n\
      		} else {\n\
      	 that.AJAX.onreadystatechange = function() {\n\
      	 	if (that.AJAX.readyState==4) {\n\
      	 		that.updating=false;\n\
      	 		that.callback(that.AJAX.responseText,that.AJAX.status,that.AJAX.responseXML);\n\
      	 		that.AJAX=null;\n\
      	 	}\n\
      	 }\n\
      	 that.updating = new Date();\n\
      	 if (/post/i.test(postMethod)) {\n\
      	 	var uri=urlCall+'?'+that.updating.getTime();\n\
      	 	that.AJAX.open(\"POST\", uri, true);\n\
      	 	that.AJAX.setRequestHeader(\"Content-type\", \"application/x-www-form-urlencoded\");\n\
      	 	that.AJAX.setRequestHeader(\"Content-Length\", passData.length);\n\
      	 	that.AJAX.send(passData);\n\
      	 } else {\n\
      	 	var uri=urlCall+'?'+passData+'&timestamp='+(that.updating.getTime());\n\
      	 	that.AJAX.open(\"GET\", uri, true);\n\
      	 	that.AJAX.send(null);\n\
      	 }\n\
      	 return true;\n\
      		}\n\
      	}\n\
      	var urlCall = url;\n\
      	this.callback = callbackFunction || function () { };\n\
      }\n\
\n\
      function processData(JSONData) {\n\
      	 if (buttonstart.disabled) {\n\
      	ajaxisrunning = true;\n\
\n\
      	avviso.firstChild.data=\"Arduino connected\";\n\
\n\
      	if (JSONData.IO2 == \"H\") {\n\
      		IO2.style.backgroundColor=\"green\";\n\
      	        IO2.firstChild.data=port_high;\n\      	       
      	} else {\n\
      		IO2.style.backgroundColor=\"red\";\n\
      	        IO2.firstChild.data=port_low;\n\
      	}\n\
\n\
      	if (JSONData.IO3 == \"H\") {\n\
      		IO3.style.backgroundColor=\"green\";\n\
      	        IO3.firstChild.data=port_high;\n\
      	} else {\n\
      		IO3.style.backgroundColor=\"red\";\n\
      	        IO3.firstChild.data=port_low;\n\
      	}\n\
\n\
      	if (JSONData.IO5 == \"H\") {\n\
      		IO5.style.backgroundColor=\"green\";\n\
      	        IO5.firstChild.data=port_high;\n\
      	} else {\n\
      		IO5.style.backgroundColor=\"red\";\n\
      	        IO5.firstChild.data=port_low;\n\
      	}\n\
\n\
      	if (JSONData.IO6 == \"H\") {\n\
      		IO6.style.backgroundColor=\"green\";\n\
      	        IO6.firstChild.data=port_high;\n\
      	} else {\n\
      		IO6.style.backgroundColor=\"red\";\n\
      	        IO6.firstChild.data=port_low;\n\
      	}\n\
\n\
      	if (JSONData.IO7 == \"H\") {\n\
      		IO7.style.backgroundColor=\"green\";\n\
      	        IO7.firstChild.data=port_high;\n\
      	} else {\n\
      		IO7.style.backgroundColor=\"red\";\n\
      	        IO7.firstChild.data=port_low;\n\
      	}\n\
\n\
      	if (JSONData.IO8 == \"H\") {\n\
      		IO8.style.backgroundColor=\"green\";\n\
      	        IO8.firstChild.data=port_high;\n\
      	} else {\n\
      		IO8.style.backgroundColor=\"red\";\n\
      	        IO8.firstChild.data=port_low;\n\
      	}\n\
\n\
      	if (JSONData.IO9 == \"H\") {\n\
      		IO9.style.backgroundColor=\"green\";\n\
      	        IO9.firstChild.data=port_high;\n\
      	} else {\n\
      		IO9.style.backgroundColor=\"red\";\n\
      	        IO9.firstChild.data=port_low;\n\
      	}\n\
\n\
      	AD0.firstChild.data=JSONData.AD0;\n\
      	PAD0.value=JSONData.AD0;\n\
      	if (JSONData.AD0 > midanalog) {\n\
      		AD0.style.backgroundColor=\"green\";\n\
      	} else {\n\
      		AD0.style.backgroundColor=\"red\";\n\
      	}\n\
\n\
      	AD1.firstChild.data=JSONData.AD1;\n\
      	PAD1.value=JSONData.AD1;\n\
      	if (JSONData.AD1 > midanalog) {\n\
      		AD1.style.backgroundColor=\"green\";\n\
      	} else {\n\
      		AD1.style.backgroundColor=\"red\";\n\
      	}\n\
\n\
      	AD2.firstChild.data=JSONData.AD2;\n\
      	PAD2.value=JSONData.AD2;\n\
      	if (JSONData.AD2 > midanalog) {\n\
      		AD2.style.backgroundColor=\"green\";\n\
      	} else {\n\
      		AD2.style.backgroundColor=\"red\";\n\
      	}\n\
\n\
      	AD3.firstChild.data=JSONData.AD3;\n\
      	PAD3.value=JSONData.AD3;\n\
      	if (JSONData.AD3 > midanalog) {\n\
      		AD3.style.backgroundColor=\"green\";\n\
      	} else {\n\
      		AD3.style.backgroundColor=\"red\";\n\
      	}\n\
\n\
      	AD4.firstChild.data=JSONData.AD4;\n\
      	PAD4.value=JSONData.AD4;\n\
      	if (JSONData.AD4 > midanalog) {\n\
      		AD4.style.backgroundColor=\"green\";\n\
      	} else {\n\
      		AD4.style.backgroundColor=\"red\";\n\
      	}\n\
\n\
      	AD5.firstChild.data=JSONData.AD5;\n\
      	PAD5.value=JSONData.AD5;\n\
      	if (JSONData.AD5 > midanalog) {\n\
      		AD5.style.backgroundColor=\"green\";\n\
      	} else {\n\
      		AD5.style.backgroundColor=\"red\";\n\
      	}\n\
\n\
      	var d1 = new Date();\n\
      	lastexecutiontime=d1.getTime();\n\
\n\
      	ajaxisrunning = false;\n\
      	 }\n\
      }\n\
\n\
      function cambiacolore(colore) {\n\
      	IO2.style.backgroundColor=colore;\n\
      	 IO2.firstChild.data=\"XX\";\n\
      	IO3.style.backgroundColor=colore;\n\
      	 IO3.firstChild.data=\"XX\";\n\
      	IO5.style.backgroundColor=colore;\n\
      	 IO5.firstChild.data=\"XX\";\n\
      	IO6.style.backgroundColor=colore;\n\
      	 IO6.firstChild.data=\"XX\";\n\
      	IO7.style.backgroundColor=colore;\n\
      	 IO7.firstChild.data=\"XX\";\n\
      	IO8.style.backgroundColor=colore;\n\
      	 IO8.firstChild.data=\"XX\";\n\
      	IO9.style.backgroundColor=colore;\n\
      	 IO9.firstChild.data=\"XX\";\n\
      	AD0.style.backgroundColor=colore;\n\
      	 AD0.firstChild.data=\"XX\";\n\
      	 PAD0.value=0;\n\
      	AD1.style.backgroundColor=colore;\n\
      	 AD1.firstChild.data=\"XX\";\n\
      	 PAD1.value=0;\n\
      	AD2.style.backgroundColor=colore;\n\
      	 AD2.firstChild.data=\"XX\";\n\
      	 PAD2.value=0;\n\
      	AD3.style.backgroundColor=colore;\n\
      	 AD3.firstChild.data=\"XX\";\n\
      	 PAD3.value=0;\n\
      	AD4.style.backgroundColor=colore;\n\
      	 AD4.firstChild.data=\"XX\";\n\
      	 PAD4.value=0;\n\
      	AD5.style.backgroundColor=colore;\n\
      	 AD5.firstChild.data=\"XX\";\n\
      	 PAD5.value=0;\n\
\n\
      }\n\
\n\
    function abilitabottoni(){\n\
\n\
    IDB2.disabled=false;\n\
    IDB3.disabled=false;\n\
    IDB5.disabled=false;\n\
    IDB6.disabled=false;\n\
    IDB7.disabled=false;\n\
    IDB8.disabled=false;\n\
    IDB9.disabled=false;\n\
    }\n\
\n\
    function disabilitabottoni(){\n\
\n\
    IDB2.disabled=true;\n\
    IDB3.disabled=true;\n\
    IDB5.disabled=true;\n\
    IDB6.disabled=true;\n\
    IDB7.disabled=true;\n\
    IDB8.disabled=true;\n\
    IDB9.disabled=true;\n\
    }\n\
\n\
      function fai() {\n\
\n\
      	var d1 = new Date();\n\
      	var adesso=d1.getTime();\n\
      	var miadifferenza=adesso-lastexecutiontime;\n\
      	momento_differenza.firstChild.data=miadifferenza + \" ms from last refresh\";\n\
\n\
      	if ( miadifferenza > tempomassimo ) {\n\
      		cambiacolore(\"yellow\");\n\
      		avviso.firstChild.data=\"Arduino connection error!\";\n\
      	         disabilitabottoni();\n\
      	} else {\n\
      	         abilitabottoni();\n\
      	 }\n\
\n\
      	var ajaxRequest = new ajaxObject('/json');\n\
      	ajaxRequest.callback = function (responseText) {\n\
      		eval(responseText);\n\
      	}\n\
      	ajaxRequest.update();\n\
      }\n\
\n\
      function bottone(porta, scritta) {\n\
\n\
      	var azione;\n\
      	 var ajaxRequest = new ajaxObject('/bottone' + porta);\n\
      	ajaxRequest.callback = function (responseText) {\n\
      		eval(responseText);\n\
      	}\n\
      	ajaxRequest.update();\n\
      }\n\
\n\
      function ferma() {\n\
      	clearInterval(intervallo);\n\
      	buttonstart.disabled=false;\n\
      	while (ajaxisrunning) {\n\
// aspetta che la funzione di json lanciata da arduino termini e poi cancella il messaggio\n\
      	}\n\
      	cambiacolore(\"aquamarine\");\n\
      	avviso.firstChild.data=\"Suspended app, to run click on Start\";\n\
      	momento_differenza.firstChild.data=\"X\";\n\      	
      	buttonferma.disabled=true;\n\
      	disabilitabottoni();\n\
      }\n\
\n\
      function avvia() {\n\
      	var d1 = new Date();\n\
      	lastexecutiontime=d1.getTime();\n\
      	intervallo = setInterval('fai()',ciclo);\n\
      	buttonferma.disabled=false;\n\
      	buttonstart.disabled=true;\n\
      	messaggio.firstChild.data=\"Refresh time \" + ciclo + \" ms, timeout \" + tempomassimo + \" ms\";\n\
      	abilitabottoni();\n\
      }\n\
\n\
      function inizia() {\n\
//      	message.firstChild.data=\"Refresh time \" + ciclo + \" Timeout time \" + tempomassimo;\n\
      	 ferma();\n\
      	avvia();\n\
      }\n\
//-->\n\
</SCRIPT>\n\
	</head>\n\
		<body onload=\"javascript:inizia();\">\n\
      <div align=\"center\">\n\
    <h1>Arduino ports switch</h1>\n\
      <div id=\"momento_differenza\">milliseconds from last refresh</div>\n\
    <div id=\"avviso\">Suspended app, to run click on Start<div><br>\n\
		<FORM>\n\
      <INPUT type=\"button\" value=\"Start\" onClick=\"javascript:avvia();\" disabled=\"true\" id=\"buttonstart\">\n\
      <INPUT type=\"button\" value=\"Stop\" onClick=\"javascript:ferma();\" id=\"buttonferma\">\n\
		</FORM>\n\
		<table border=\"1\">\n\
    <tr>\n\
    	<th>Digital Outputs</th>\n\
      	<th>Status</th>\n\
      	 <th>Switch</th>\n\
    </tr>\n\
    <tr>\n\
      	<td align=\"center\">2</td>\n\
      	<td id=\"IO2\" align=\"center\">XX</td>\n\
      	 <td align=\"center\"><INPUT type=\"button\" value=\"Switch\" onClick=\"javascript:bottone('2')\" id=\"IDB2\">\n\
      </tr>\n\
      <tr>\n\
      	<td align=\"center\">3</td>\n\
      	<td id=\"IO3\" align=\"center\">XX</td>\n\
      	 <td align=\"center\"><INPUT type=\"button\" value=\"Switch\" onClick=\"javascript:bottone('3');\" id=\"IDB3\">\n\
      </tr>\n\
      <tr>\n\
      	<td align=\"center\">5</td>\n\
      	<td id=\"IO5\" align=\"center\">XX</td>\n\
      	 <td align=\"center\"><INPUT type=\"button\" value=\"Switch\" onClick=\"javascript:bottone('5');\" id=\"IDB5\">\n\
      </tr>\n\
      <tr>\n\
      	<td align=\"center\">6</td>\n\
      	<td id=\"IO6\" align=\"center\">XX</td>\n\
      	 <td align=\"center\"><INPUT type=\"button\" value=\"Switch\" onClick=\"javascript:bottone('6');\" id=\"IDB6\">\n\
      </tr>\n\
      <tr>\n\
      	<td align=\"center\">7</td>\n\
      	<td id=\"IO7\" align=\"center\">XX</td>\n\
      	 <td align=\"center\"><INPUT type=\"button\" value=\"Switch\" onClick=\"javascript:bottone('7');\" id=\"IDB7\">\n\
      </tr>\n\
      <tr>\n\
      	<td align=\"center\">8</td>\n\
      	   <td id=\"IO8\" align=\"center\">XX</td>\n\
      	 <td align=\"center\"><INPUT type=\"button\" value=\"Switch\" onClick=\"javascript:bottone('8');\" id=\"IDB8\">\n\
      </tr>\n\
      <tr>\n\
      	<td align=\"center\">9</td>\n\
      	<td id=\"IO9\" align=\"center\">XX</td>\n\
      	 <td align=\"center\"><INPUT type=\"button\" value=\"Switch\" onClick=\"javascript:bottone('9');\" id=\"IDB9\">\n\
      </tr>\n\
	         </table>\n\
	<br>\n\
	<br>\n\
	<table border=\"1\">\n\
		<tr>\n\
    <th align=\"center\">Analogs inputs</th>\n\
	      	     <th align=\"center\">Values</th>\n\
    <th align=\"center\">Bars</th>\n\
      	     </tr>\n\
      	     <tr>\n\
      <td align=\"center\">0</td>\n\
      <td id=\"AD0\" align=\"center\">NO</td>\n\
      <td><progress value=\"0\" max=\"1023\" id=\"PAD0\"></progress></td>\n\
		</tr>\n\
		<tr>\n\
      <td align=\"center\">1</td>\n\
      <td id=\"AD1\" align=\"center\">NO</td>\n\
      <td><progress value=\"0\" max=\"1023\" id=\"PAD1\"></progress></td>\n\
		</tr>\n\
		<tr>\n\
      <td align=\"center\">2</td>\n\
      <td id=\"AD2\" align=\"center\">NO</td>\n\
      <td><progress value=\"0\" max=\"1023\" id=\"PAD2\"></progress></td>\n\
		</tr>\n\
		<tr>\n\
      <td align=\"center\">3</td>\n\
      <td id=\"AD3\" align=\"center\">NO</td>\n\
      <td><progress value=\"0\" max=\"1023\" id=\"PAD3\"></progress></td>\n\
		</tr>\n\
		<tr>\n\
      <td align=\"center\">4</td>\n\
      <td id=\"AD4\" align=\"center\">NO</td>\n\
      <td><progress value=\"0\" max=\"1023\" id=\"PAD4\"></progress></td>\n\
		</tr>\n\
		<tr>\n\
      <td align=\"center\">5</td>\n\
      <td id=\"AD5\" align=\"center\">NO</td>\n\
      <td><progress value=\"0\" max=\"1023\" id=\"PAD5\"></progress></td>\n\
		</tr>\n\
	</table>\n\
      	     <br><br><div id=\"messaggio\"> </div>\n\
      	     <br>Don't exaggerate with the number of client, Arduino has an 8 bit 16 MHz cpu!\n\ 
		</div>\n\
	</body>\n\
</html>");
  #ifdef DEBUG
    Serial << F("Sent home\n");
  #endif
  return true;
}

boolean has_ip_address = false;
MyTinyWebServer web = MyTinyWebServer(handlers, NULL);

const char* ip_to_str(const uint8_t* ipAddr)
{
  static char buf[16];
  sprintf(buf, "%d.%d.%d.%d\0", ipAddr[0], ipAddr[1], ipAddr[2], ipAddr[3]);
  return buf;
}

void setup() {
  
  #ifdef DEBUG
    Serial.begin(9600);
    Serial << F("Free RAM: ") << FreeRam() << "\n";
  #endif
  
  pinMode(SS_PIN, OUTPUT);	// set the SS pin as an output
      	 // (necessary to keep the board as
      	 // master and not SPI slave)
  digitalWrite(SS_PIN, HIGH);	// and ensure SS is high

  // Ensure we are in a consistent state after power-up or a reset
  // button These pins are standard for the Arduino w5100 Rev 3
  // ethernet board They may need to be re-jigged for different boards
  pinMode(ETHER_CS, OUTPUT);	// Set the CS pin as an output
  digitalWrite(ETHER_CS, HIGH);	// Turn off the W5100 chip! (wait for
      	 // configuration)
  pinMode(SD_CS, OUTPUT);	// Set the SDcard CS pin as an output
  digitalWrite(SD_CS, HIGH);	// Turn off the SD card! (wait for
      	 // configuration)

  for (byte i=0; i < ioportsnum; i++) {
    pinMode(ioports[i], OUTPUT);
  }
    
  // Initialize the Ethernet.
  #ifdef DEBUG
    Serial << F("Setting up the Ethernet card...\n");
  #endif
  
  Ethernet.begin(mac, ip);

  // Start the web server.
  #ifdef DEBUG
    Serial << F("Web server starting...\n");
  #endif  
  web.begin();
  #ifdef DEBUG
    Serial << F("Ready to accept HTTP requests.\n\n");
  #endif
}

void loop() {
  ioportsStatus[actualread] = digitalRead(ioports[actualread]);
  actualread++;
  if (actualread >= ioportsnum) {
    actualread = 0;
  }
  
  unsigned int midread = 0;
  for (byte i=0; i <= 10; i++) {
    midread = midread + analogRead(actualanalog);
  }
  analogValues[actualanalog] = midread / 10;
  actualanalog++;
  if (actualanalog > 5) {
    actualanalog = 0;
  }
  web.process();
}
