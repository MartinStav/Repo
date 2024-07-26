function createPriserka() {
  let priserka = new Priserka();
  player.addObserver(priserka);
}

function createScore() {
  let score = new Score();
  player.addObserverScore(score);
}

function createEnemy() {
  let enemy = new Enemy();
  if (soundOn) {
    enemySpawn.currentTime = 1.7;
    enemySpawn.play();
  }
  for (let i = 1, j = 0; i < 5; i++) {
    if (i > 2) {
      j += 2;
    }
    setTimeout(() => {
      player.priserky[enemy.enemySpawn].animPos = i - j;
    }, 200 * i);
  }
  setTimeout(() => {
    player.addObserverEnemy(enemy);
  }, 500);
}

function getTime() {
  endTime = new Date();
  let timeDiff = endTime - startTime;
  timeDiff /= 1000;

  seconds = Math.round(timeDiff);
  return seconds;
}

document.getElementById("leaderboardButton").addEventListener("click", () => {
  displayLeaderboard();
});

function endGameFunction() {
  if (levelSelected == 2) {
    let playerName = document.getElementById("name").value;

    if (playerName == "") {
      playerName = "Guest" + (Math.floor(Math.random() * 999) + 1);
    }

    let playerData = playerName + "," + scoreNum + "," + getTime();
    let leaderboard = sessionStorage.getItem("leaderboard") || "";
    leaderboard += playerData + ";";
    sessionStorage.setItem("leaderboard", leaderboard);

    displayLeaderboard();
  }

  scoreNum = 0;
  longerGame = 0;
  player.removeObserver();
  player.removeObserverEnemy();
  player.removeObserverScore();
  player.x = canvas.width / 2;
  player.y = canvas.height / 2;
  mainMenu.style.display = "flex";
  pauseMenu.style.display = "none";
  cursor ? (canvas.style.cursor = "auto") : (canvas.style.cursor = "none");
  cursor = true;
}

function displayLeaderboard() {
  let leaderboard = sessionStorage.getItem("leaderboard");
  leaderboard = leaderboard.split(";").filter((entry) => entry != "");

  leaderboard.sort(function (a, b) {
    let scoreA = a.split(",")[1];
    let scoreB = b.split(",")[1];
    return scoreB - scoreA;
  });

  let leaderboardHTML = "<ol>";
  for (let i = 0; i < Math.min(leaderboard.length, 10); i++) {
    let playerData = leaderboard[i].split(",");
    leaderboardHTML +=
      "<li><b>" +
      playerData[0] +
      "</b> - " +
      playerData[1] * 100 +
      " points in " +
      playerData[2] +
      " seconds</li>";
  }
  leaderboardHTML += "</ol>";
  document.getElementById("leaderboardTable").innerHTML = leaderboardHTML;
}

for (let i = 0; i < menuButton.length; i++) {
  menuButton[i].addEventListener("click", () => {
    winScreen.style.display = "none";
    gameOver.style.display = "none";
    endGameFunction();
    gameEnd = false;
  });
}

for (let i = 0; i < restartButton.length; i++) {
  restartButton[i].addEventListener("click", () => {
    endGameFunction();
    cursor = false;
    cursor ? (canvas.style.cursor = "auto") : (canvas.style.cursor = "none");
    createPriserka();
    startTime = new Date();
    gameEnd = false;
    paused = false;
    cursor = false;
    frames = 0;
    framesEnemy = 0;
    mainMenu.style.display = "none";
    playMenu.style.display = "none";
    winScreen.style.display = "none";
    gameOver.style.display = "none";
    player.mouseX = canvas.width / 2;
    player.mouseY = canvas.height / 2;
  });
}

for (let i = 0; i < musicButton.length; i++) {
  musicButton[i].addEventListener("click", () => {
    musicOn = !musicOn;
    musicOn ? music.play() : music.pause();
    for (let j = 0; j < musicButton.length; j++) {
      musicOn
        ? (musicButton[j].style.background = "var(--primary)")
        : (musicButton[j].style.background = "var(--primary-off)");
    }
  });
}
window.addEventListener("click", () => {
  if (musicFix == false) {
    musicFix = true;
    musicOn ? music.play() : music.pause();
  }
});

for (let i = 0; i < soundButton.length; i++) {
  soundButton[i].addEventListener("click", () => {
    soundOn = !soundOn;
    for (let j = 0; j < soundButton.length; j++) {
      soundOn
        ? (soundButton[j].style.background = "var(--primary)")
        : (soundButton[j].style.background = "var(--primary-off)");
    }
  });
}

for (let i = 0; i < levelPick.length; i++) {
  levelPick[i].addEventListener("click", () => {
    levelSelected = i;
    createPriserka();
    startTime = new Date();
    gameEnd = false;
    paused = false;
    cursor = false;
    frames = 0;
    framesEnemy = 0;
    mainMenu.style.display = "none";
    playMenu.style.display = "none";
    cursor ? (canvas.style.cursor = "auto") : (canvas.style.cursor = "none");
    player.mouseX = canvas.width / 2;
    player.mouseY = canvas.height / 2;
  });
}
playButton.addEventListener("click", () => {
  playMenu.style.display = "flex";
});

settingsButton.addEventListener("click", () => {
  settingsMenu.style.display = "flex";
});

leaderboardButton.addEventListener("click", () => {
  leaderboardMenu.style.display = "flex";
});

howtoButton.addEventListener("click", () => {
  howtoMenu.style.display = "flex";
});

for (let i = 0; i < menuBack.length; i++) {
  menuBack[i].addEventListener("click", () => {
    howtoMenu.style.display = "none";
    playMenu.style.display = "none";
    settingsMenu.style.display = "none";
    leaderboardMenu.style.display = "none";
  });
}

pauseBack.addEventListener("click", () => {
  paused = false;
  cursor = false;
  cursor ? (canvas.style.cursor = "auto") : (canvas.style.cursor = "none");
  pauseMenu.style.display = "none";
});

endGame.addEventListener("click", () => {
  endGameFunction();
});

canvas.addEventListener("mousemove", (event) => {
  player.mouseX = event.clientX - rect.left;
  player.mouseY = event.clientY - rect.top;
});

function OverFunction() {
  gameEnd = true;
  paused = true;
  gameOver.style.display = "flex";
  getTime();
  if (soundOn) {
    deathSound.play();
    deathSound.currentTime = 0;
    music.volume = 0.05;
    player.x = canvas.width / 2;
    player.y = canvas.height / 2;
    setTimeout(() => {
      music.volume = 0.3;
    }, 4000);
  }
  player.removeObserver();
  player.removeObserverEnemy();
}

canvas.addEventListener("click", (event) => {
  if (player.x >= 10 && player.x <= 50 && player.y >= 10 && player.y <= 50) {
    cursor ? (canvas.style.cursor = "auto") : (canvas.style.cursor = "none");
    paused = !paused;
    cursor = !cursor;
    pauseMenu.style.display = "flex";
  }
});

function collision(x, y, size, thisX, thisY, thisSize) {
  let A = thisX + thisSize / 2 - x;
  let B = thisY + thisSize / 2 - y;
  let distance = Math.sqrt(A * A + B * B);
  if (distance < size / 2 + thisSize / 2) {
    return true;
  }
}
