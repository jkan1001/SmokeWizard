const char webpageCode[] = 
R"=====(
<!DOCTYPE html>
<html>
    <head>
        <meta name="viewport" content="width=device-width, initial-scale=1">
        <link rel="icon" href="data:,">
        <style>
            html {font-family: monospace; display: inline-block; margin: 0px auto; text-align: center;}
            .button
            { 
                background-color: yellowgreen;
                border: none; 
                color: white; 
                padding: 16px 40px;
                text-decoration: none; 
                font-size: 32px; 
                margin: 2px; 
                cursor: pointer;
            }
            .button2 {background-color: gray;}
        </style>
    </head>

    <body>
        <h1>Smoke Wizard Remote Monitor &#129497</h1>
        <p>See how your brisket's doing</p>

        <!--
        if (statePin16 == "off") {
            <p><a href="/16/on"><button class="button">ON</button></a></p>
        } else {
            <p><a href="/16/off"><button class="button button2">OFF</button></a></p>
        }
        if (statePin17 == "off") {
            <p><a href="/17/on"><button class="button">ON</button></a></p>
        } else {
            <p><a href="/17/off"><button class="button button2">OFF</button></a></p>
        }
    -->
    </body>
</html>

)=====";