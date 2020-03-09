#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#define R D5
#define G D6
#define B D8

#define SSID "Lolspot"
#define PASSWORD "One2Three4"

#define SERVER_PORT 80

ESP8266WebServer server(SERVER_PORT);

void handleRoot()
{
        String red = server.arg(0);
        String green = server.arg(1);
        String blue = server.arg(2);

        analogWrite(R, red.toInt());
        analogWrite(G, green.toInt());
        analogWrite(B, blue.toInt());

        String webpage = String("<!DOCTYPE HTML>") +
        "<html>" +

        "<head>" +
        "<meta name='mobile-web-app-capable' content='yes' />" +
        "<meta name='viewport' content='width=device-width' />" +
        "</head>" +

        "<body style='margin: 0px; padding: 0px;'>" +
        "<br></br>" +
        "<label for='red'>R:</label>" +
        "<input min='0' max='1023' type='text' id='red_input' name='red'>" +
        "<label for='green'>G:</label>" +
        "<input min='0' max='1023' type='text' id='green_input' name='green'>" +
        "<label for='blue'>B:</label>" +
        "<input min='0' max='1023' type='text' id='blue_input' name='blue'>" +
        "<input type='submit'></input>" +
        "<br></br>" +
        "<canvas id='colorspace'></canvas>" +
        "</body>" +

       "<script type='text/javascript'>" +
        "(function() {" + 
                "var canvas = document.getElementById('colorspace');" +
                "var ctx = canvas.getContext('2d');" +

                "function drawCanvas() {" +
                        "var colours = ctx.createLinearGradient(0, 0, window.innerWidth, 0);" +
                        "for (var i = 0; i <= 360; i += 10) {" +
                        "        colours.addColorStop(i / 360, 'hsl(' + i + ', 100%, 50%)');" +
                        "}" +

                        "ctx.fillStyle = colours;" +
                        "ctx.fillRect(0, 0, window.innerWidth, window.innerHeight);" +
                        "var luminance = ctx.createLinearGradient(0, 0, 0, ctx.canvas.height);" +
                        "luminance.addColorStop(0, '#ffffff');" +
                        "luminance.addColorStop(0.05, '#ffffff');" +
                        "luminance.addColorStop(0.5, 'rgba(0,0,0,0)');" +
                        "luminance.addColorStop(0.95, '#000000');" +
                        "luminance.addColorStop(1, '#000000');" +
                        "ctx.fillStyle = luminance;" +
                        "ctx.fillRect(0, 0, ctx.canvas.width, ctx.canvas.height);" +
                "}" +
                "var eventLocked = false;" +

                "function handleEvent(clientX, clientY) {" +
                        "if (eventLocked) {" +
                        "        return;" +
                        "}" +

                        "function colourCorrect(v) {" +
                                "return Math.round((v * v) / 64);" +
                        "}" +

                        "var data = ctx.getImageData(clientX, clientY, 1, 1).data;" +
                        "var r_value = colourCorrect(data[0]);" +
                        "var g_value = colourCorrect(data[1]);" +
                        "var b_value = colourCorrect(data[2]);" +
                        
                        "document.getElementById('red_input').value = r_value;" +
                        "document.getElementById('green_input').value = g_value;" +
                        "document.getElementById('blue_input').value = b_value;" +

                        "var params = [" +
                                "'r=' + colourCorrect(r_value)," +
                                "'g=' + colourCorrect(g_value)," +
                                "'b=' + colourCorrect(b_value)" +
                        "].join('&');" +

                        "var req = new XMLHttpRequest();" + 
                        "req.open('POST', '?' + params, true);" + 
                        "req.send();" + 
                        "eventLocked = true;" + 
                        "" + 
                        "req.onreadystatechange = function() {" + 
                        "        if (req.readyState == 4) {" + 
                        "        eventLocked = false;" + 
                        "        }" + 
                        "}" + 
                "}" +

                "canvas.addEventListener('click', function(event) {" +
                "        handleEvent(event.clientX, event.clientY, true);" +
                "}, false);" +

                "canvas.addEventListener('touchmove', function(event) {" +
                "        handleEvent(event.touches[0].clientX, event.touches[0].clientY);" +
                "}, false);" +

                "function resizeCanvas() {" +
                        "canvas.width = window.innerWidth;" +
                        "canvas.height = window.innerHeight;" +
                        "drawCanvas();" +
                "}" +

                "window.addEventListener('resize', resizeCanvas, false);" +
                "resizeCanvas();" +
                "drawCanvas();" +

                "document.ontouchmove = function(e) {" +
                        "e.preventDefault()" +
                "};" +
        "})();" +
        "</script>" + 
        "<html>";

        server.send(200, "text/html", webpage);
}

void fade(int pin)
{
        uint16_t i = 0;

        for (i = 0; i < 1024; i++) {
                analogWrite(pin,  i);
                delay(1);
        }
        
        for (; i >= 0; i++) {
                analogWrite(pin, i);
                delay(1);
        }
}

/* fade in and out of Red, Green, Blue */
void testRGB()
{
        analogWrite(R, 0); // R off
        analogWrite(G, 0); // G off
        analogWrite(B, 0); // B off 
        fade(R);           // R
        fade(G);           // G
        fade(B);           // B
}

void handleNotFound()
{
        //digitalWrite(led, 1);
        
        String message = "File Not Found\n\n";
        
        message += "URI: ";
        message += server.uri();
        message += "\nMethod: ";
        message += (server.method() == HTTP_GET)?"GET":"POST";
        message += "\nArguments: ";
        message += server.args();
        message += "\n";

        for (uint8_t i = 0; i < server.args(); i++)
                message += " " + server.argName(i) + ": " + server.arg(i) + "\n";

        server.send(404, "text/plain", message);
        //digitalWrite(led, 0);
}

void setup(void)
{
        pinMode(R, OUTPUT);
        pinMode(G, OUTPUT);
        pinMode(B, OUTPUT);

        analogWrite(R, 0); // R 
        analogWrite(G, 0); // G 
        analogWrite(B, 0); // B 

        Serial.begin(115200);
        WiFi.begin(SSID, PASSWORD);
        Serial.println("");

        // Wait for connection
        while (WiFi.status() != WL_CONNECTED) {
                delay(500);
                Serial.print(".");
        }

        Serial.println("");
        Serial.print("Connected to ");
        Serial.println(SSID);
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());

        if (MDNS.begin("esp8266"))
                Serial.println("MDNS responder started");

        server.on("/", handleRoot);
        testRGB();

        server.onNotFound(handleNotFound);
        server.begin();
        Serial.println("HTTP server started");
}

void loop(void)
{
  server.handleClient();
}