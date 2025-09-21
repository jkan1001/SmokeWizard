const form = document.getElementById("wifi-setup-form");
form.addEventListener('submit', function (event) {
    // prevent form from automatically sending the POST request so we can handle it in this script
    event.preventDefault();
    // get form data
    const formData = new FormData(form);
    const ssid = formData.get("ssid");
    const pass = formData.get("pass");
    const ssid_arr = ssid.split('');
    const pass_arr = pass.split('');
    var arr = new Array(64);
    for (let i = 0; i < 64; i++) {
        if (i < ssid_arr.length) {
            arr[i] = ssid_arr[i];
        } else if (i == ssid_arr.length) {
            arr[i] = ',';
        } else if ((i > ssid_arr.length) && (i < (ssid_arr.length + 1 + pass_arr.length))) {
            arr[i] = pass_arr[i - (ssid_arr.length + 1)];
        }
    }
    const formString = arr.join("");

    fetch('/credPost.html', {
        method: 'POST',
        redirect: 'follow',
        body: formString
    })
        .then(response => {
            console.log(response);
            window.location.href = response.url; // actually redirect the user to index.html
        })
        .then(data => {
            console.log("Server response:", data);
            // If it's 200 (OK), the ESP32 connected successfully to the Wi-Fi network.
            // Client needs to get new, local IP address from server (in data object)
            // and either:
            // a) tell the user to connect to the new IP address that's on the LAN, or
            // b) somehow redirect them automatically
            
            // e.g.
            // const newIP = data.new_ip;
            // window.location.replace = newIP;
            
            // If it's 500 (NOT OK), then ask the user to try again with a pop-up
            // alert("Error code 500. Smoker failed to connect with given credentials. Please try again.");
        })
        .catch(error => {
            console.error('Error:', error);
        });
})