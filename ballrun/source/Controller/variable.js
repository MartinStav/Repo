// HMTL
const mainMenu = document.getElementById("mainMenu");
const playButton = document.getElementById("playButton");
const musicButton = document.querySelectorAll(".musicButton");
const soundButton = document.querySelectorAll(".soundButton");
const pauseMenu = document.getElementById("pauseMenu");
const pauseBack = document.getElementById("pauseBack");
const endGame = document.getElementById("endGame");
const settingsMenu = document.getElementById("settingsMenu");
const settingsButton = document.getElementById("settingsButton");
const menuBack = document.querySelectorAll(".menuBack");
const howtoMenu = document.getElementById("howtoMenu");
const howtoButton = document.getElementById("howtoButton");
const gameOver = document.getElementById("gameOver");
const restartButton = document.querySelectorAll(".restartButton");
const menuButton = document.querySelectorAll(".menuButton");
const winScreen = document.getElementById("winScreen");
const winVideo = document.getElementById("winVideo");
const playMenu = document.getElementById("playMenu");
const levelPick = document.querySelectorAll(".levelPick");
const leaderboardButton = document.getElementById("leaderboardButton");
const leaderboardMenu = document.getElementById("leaderboardMenu");

// JS
const canvas = document.getElementById("canvas");
const ctx = canvas.getContext("2d");
const playerSize = 30;
let music = new Audio("../../assets/sound/music.mp3");
music.volume = 0.3;
music.play();
let deathSound = new Audio("../../assets/sound/death-sound.mp3");
deathSound.volume = 0.5;
deathSound.playbackRate = 1.6;
deathSound.loop = false;
let musicOn = true;
let soundOn = true;
let paused = true;
let cursor = true;
let gameEnd = false;
let musicFix = false;
const rect = canvas.getBoundingClientRect();
let enemySpawn = new Audio("../../assets/sound/enemy-spawn.mp3");
enemySpawn.volume = 0.6;
enemySpawn.loop = false;
let frames = 0;
let framesEnemy = 0;
const player = new Player();
let scoreNum = 0;
let seconds = 0;
let time = 0;
let longerGame = 0;
let levelSelected = 0;
let invincible = false;

// Levels

let level = {
  enemy_speed: [1, 2, 3],
  enemy_size: [25, 27.5, 30],
  priserka_number: [1, 3, 4],
  score_size: [27.5, 24, 20],
  player_size: [25, 27.5, 30],
};
