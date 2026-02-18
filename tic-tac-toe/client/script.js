// script.js - Corrected Version

let ws = null;
let playerSymbol = null;
let currentBoard = "         "; // 9 REGULAR spaces
let gameActive = false;
let myTurn = false;

// UI elements
const connectionStatus = document.getElementById("connection-status");
const symbolDisplay = document.getElementById("player-symbol");
const gameStatus = document.getElementById("game-status");
const cells = document.querySelectorAll(".cell");

function connect() {
    // Use 127.0.0.1 to avoid local DNS delay
    ws = new WebSocket("ws://127.0.0.1:9001");

    ws.onopen = function () {
        console.log("Connected to server");
        connectionStatus.textContent = "Connected";
    };

    ws.onmessage = function (event) {
        console.log("Received:", event.data);
        const msg = event.data;

        if (msg.startsWith("SYMBOL:")) {
            playerSymbol = msg.split(":")[1];
            symbolDisplay.textContent = "Symbol: " + playerSymbol;
        } 
        else if (msg === "GAME START") {
            gameActive = true;
            if (playerSymbol === "X") {
                myTurn = true;
                gameStatus.textContent = "Your Turn";
            } else {
                myTurn = false;
                gameStatus.textContent = "Opponent Turn";
            }
        } 
        else if (msg.startsWith("BOARD:")) {
            currentBoard = msg.substring(6);
            updateBoard();
            
            // Logic to determine turn based on board state
            const xCount = (currentBoard.match(/X/g) || []).length;
            const oCount = (currentBoard.match(/O/g) || []).length;

            if (playerSymbol === "X") {
                myTurn = (xCount === oCount);
            } else {
                myTurn = (oCount < xCount);
            }

            if (gameActive) {
                gameStatus.textContent = myTurn ? "Your Turn" : "Opponent Turn";
            }
        } 
        else if (msg.startsWith("WIN:")) {
            const winner = msg.split(":")[1];
            gameStatus.textContent = (winner === playerSymbol) ? "You Win!" : "You Lose!";
            gameActive = false;
        } 
        else if (msg === "DRAW") {
            gameStatus.textContent = "Game Draw!";
            gameActive = false;
        } 
        else if (msg === "OPPONENT LEFT") {
            gameStatus.textContent = "Opponent Left";
            gameActive = false;
        } 
        else if (msg === "WAIT FOR OPPONENT") {
            gameStatus.textContent = "Waiting for opponent...";
        }
    };

    ws.onclose = function () {
        connectionStatus.textContent = "Disconnected";
        gameStatus.textContent = "Connection lost";
        gameActive = false;
    };

    ws.onerror = function (error) {
        console.error("WebSocket error");
        connectionStatus.textContent = "Error Connecting";
    };
}

function updateBoard() {
    for (let i = 0; i < 9; i++) {
        // Only show if it's X or O, otherwise clear cell
        const char = currentBoard[i];
        cells[i].textContent = (char === 'X' || char === 'O') ? char : "";
    }
}

// Handle cell clicks
cells.forEach(cell => {
    cell.addEventListener("click", function () {
        if (!gameActive || !myTurn) return;

        const index = this.getAttribute("data-index");

        // Use a simple truthy check for the character
        if (currentBoard[index] === 'X' || currentBoard[index] === 'O') {
            return;
        }

        ws.send(index);
        myTurn = false;
        gameStatus.textContent = "Sending move...";
    });
});

// Start
connect();
