function WebSocketTest(wss_url) {
    if ("WebSocket" in window) {
        var ws = new WebSocket(wss_url);
        var curr_seq = -1;

        ws.onmessage = function (event) { 
            var received_msg = JSON.parse(event.data);

            //sync with server sequence
            if(curr_seq == -1) {
                curr_seq = --received_msg.sequence;
            }
            
            // if partial and is same message
            if(received_msg.type == 'partial') {
                if(received_msg.sequence == curr_seq) {
                    let current_child = document.getElementById('transcription_log').lastChild;
                    current_child.innerHTML = received_msg.message;
                }
                else {
                    AppendNewMessage(received_msg)
                }
            }
            else {
                AppendNewMessage(received_msg)
                ++curr_seq
            }
        };
        
        ws.onclose = function() { 
            alert("Connection is closed..."); 
        };
    } 
    else {
        alert("WebSocket NOT supported by your Browser!");
    }
}

function AppendNewMessage(msg) {
    if(msg.type == 'final') {
        var colored_marker = document.createElement('span');
        colored_marker.setAttribute("style", msg.color);
        colored_marker.innerText = "∙ ";
        document.getElementById('transcription_log').appendChild(colored_marker).scrollIntoView();
    }
    var new_msg = document.createElement('span');
    new_msg.innerText = msg.message;

    var linebreak = document.createElement('br');
    document.getElementById('transcription_log').appendChild(linebreak).scrollIntoView();

    document.getElementById('transcription_log').appendChild(new_msg).scrollIntoView();   
}