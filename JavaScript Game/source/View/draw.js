function draw() {
  if (!paused) {
    // Clear the canvas
    ctx.clearRect(0, 0, canvas.width, canvas.height);

    // Pause menu
    if (player.x >= 10 && player.x <= 50 && player.y >= 10 && player.y <= 50) {
      ctx.fillStyle = "#6DB3FC";
    } else {
      ctx.fillStyle = "#74A4D8";
    }
    ctx.fillRect(10, 10, 40, 40);

    ctx.fillStyle = "#FFFFFF";
    ctx.fillRect(16, 17, 10, 25);
    ctx.fillRect(33, 17, 10, 25);

    if (
      (levelSelected == 0 && getTime() == 20) ||
      (levelSelected == 1 && getTime() == 40)
    ) {
      document.querySelector(".time").innerText =
        Math.floor(getTime() / 100) + ":" + (getTime() % 100);
      winScreen.style.display = "flex";
      OverFunction();
    }

    //Score number
    ctx.font = "24px serif";
    if (Math.floor(getTime() % 60) < 10) {
      ctx.fillText("00:0" + (getTime() % 60), canvas.width - 100, 80);
    } else {
      ctx.fillText(
        Math.floor(getTime() / 60) + ":" + (getTime() % 60),
        canvas.width - 100,
        80
      );
    }
    ctx.fillText(scoreNum + "00", canvas.width - 100, 50);

    //Score
    if (player.scores.length < 1) {
      createScore();
    }

    //Priserka
    player.priserky.forEach((priserka) => {
      priserka.draw();
    });

    player.scores.forEach((score) => {
      score.draw();
    });

    //Enemy
    if (framesEnemy == Math.floor(110 + longerGame * 15)) {
      createEnemy();
      framesEnemy = 0;
      longerGame++;
    }
    if (frames == 300) {
      if (player.priserky.length < level.priserka_number[levelSelected]) {
        createPriserka();
      }
      frames = 0;
    }
    //Enemy
    player.enemys.forEach((enemy) => {
      enemy.draw();
    });

    // Player
    player.draw();

    frames++;
    framesEnemy++;
  }
}
if (!gameEnd) {
  window.setInterval(draw, 10);
}
