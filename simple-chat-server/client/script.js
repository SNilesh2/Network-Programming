// 1. Variables to hold our connection and UI elements
let socket = null;
const setupContainer = document.getElementById('setup-container');
const chatContainer = document.getElementById('chat-container');
const messageLog = document.getElementById('message-log');
const usernameInput = document.getElementById('username-input');
const msgInput = document.getElementById('msg-input');

// 2. Logic to Join the Chat
document.getElementById('join-btn').onclick = function() {
    const username = usernameInput.value.trim();
    if (!username) {
        alert("Please enter a username!");
        return;
    }

    // Connect to your C Server (matching the port in your main.c)
    socket = new WebSocket("ws://localhost:9001");

    // When connection is successful
    socket.onopen = function() {
        // Send the JOIN protocol string we defined in protocol.h
        socket.send("JOIN:" + username);

        // Switch UI from Login to Chat
        setupContainer.style.display = "none";
        chatContainer.style.display = "flex";
        document.getElementById('display-name').innerText = "Logged in as: " + username;
    };

    // 3. Handling incoming messages from the C Server
    socket.onmessage = function(event) {
        const rawMsg = event.data;
        displayMessage(rawMsg);
    };

    socket.onclose = function() {
        alert("Server connection closed.");
        location.reload(); // Refresh the page to go back to login
    };

    socket.onerror = function(error) {
        console.error("WebSocket Error: ", error);
        alert("Could not connect to the C server. Is it running?");
    };
};

// 4. Logic to Send a Message
document.getElementById('send-btn').onclick = sendMessage;

// Also send message if user presses "Enter" key
msgInput.onkeypress = function(e) {
    if (e.key === 'Enter') sendMessage();
};

function sendMessage() {
    const text = msgInput.value.trim();
    if (!text) return;

    // Check if it's a private message (starts with PVT:name:msg)
    if (text.startsWith("PVT:")) {
        socket.send(text); // Send raw PVT string to server
    } else {
        // Otherwise, send as a standard broadcast message
        socket.send("MSG:" + text);
    }

    msgInput.value = ""; // Clear input box
}

// 5. Logic to show the message on the screen
function displayMessage(rawMsg) {
    const newMsgDiv = document.createElement('div');
    newMsgDiv.classList.add('msg');

    // Parse the protocol prefixes
    if (rawMsg.startsWith("SYS:")) {
        newMsgDiv.classList.add('sys');
        newMsgDiv.innerText = rawMsg.substring(4);
    } 
    else if (rawMsg.startsWith("MSG:")) {
        // Format: "MSG:User:Message"
        const content = rawMsg.substring(4);
        newMsgDiv.innerText = content;
    } 
    else if (rawMsg.startsWith("PVT:")) {
        // Format: "PVT:Sender:Message"
        newMsgDiv.classList.add('pvt');
        const content = rawMsg.substring(4);
        newMsgDiv.innerText = "[Private] " + content;
    }

    messageLog.appendChild(newMsgDiv);
    
    // Auto-scroll to the bottom so we see the newest message
    messageLog.scrollTop = messageLog.scrollHeight;
}
