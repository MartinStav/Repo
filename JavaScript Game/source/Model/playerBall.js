class Player {
  constructor() {
    this.x = canvas.width / 2;
    this.y = canvas.height / 2;
    this.mouseX = this.x;
    this.mouseY = this.y;
    this.dx = this.x;
    this.dy = this.y;
    this.distance;
    this.speed = 30;
    this.size = 30;
    this.img = new Image();
    this.img.src = "../../assets/image/player.png";
    this.imgNorm = new Image();
    this.imgNorm.src = "../../assets/image/player.png";
    this.imgInvincible = new Image();
    this.imgInvincible.src = "../../assets/image/playerInvincible.png";
    this.priserky = [];
    this.enemys = [];
    this.scores = [];
  }

  draw() {
    this.dx = this.mouseX - this.x;
    this.dy = this.mouseY - this.y;
    this.distance = Math.sqrt(this.dx * this.dx + this.dy * this.dy);

    if (this.distance < this.speed) {
      this.x += this.dx;
      this.y += this.dy;
    } else {
      this.x += this.dx * (this.speed / this.distance);
      this.y += this.dy * (this.speed / this.distance);
    }

    if (invincible) {
      this.img = this.imgInvincible;
    } else {
      this.img = this.imgNorm;
    }

    ctx.drawImage(
      this.img,
      this.x - this.size / 2,
      this.y - this.size / 2,
      this.size,
      this.size
    );
    ctx.shadowColor = "rgba(0, 0, 0, 0.3)";
    ctx.shadowBlur = 10;
    this.positionChanged();
  }

  addObserver(priserka) {
    this.priserky.push(priserka);
  }

  addObserverEnemy(enemy) {
    this.enemys.push(enemy);
  }

  addObserverScore(score) {
    this.scores.push(score);
  }

  removeObserver() {
    for (let i = this.priserky.length - 1; i >= 0; i--) {
      this.priserky.splice(i, 1);
    }
  }

  removeObserverEnemy() {
    for (let i = this.enemys.length - 1; i >= 0; i--) {
      this.enemys.splice(i, 1);
    }
  }

  removeObserverScore() {
    for (let i = this.scores.length - 1; i >= 0; i--) {
      this.scores.splice(i, 1);
    }
  }

  positionChanged() {
    this.priserky.forEach((priserka) => {
      priserka.notify(this.x, this.y, this.size);
    });
    this.enemys.forEach((enemy) => {
      enemy.notify(this.x, this.y, this.size, this.scores);
    });
    this.scores.forEach((score) => {
      score.notify(this.x, this.y, this.size, this.enemys);
    });
  }
}
