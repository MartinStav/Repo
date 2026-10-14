class Score {
  constructor() {
    this.size = level.score_size[levelSelected];
    this.x =
      Math.floor(Math.random() * (canvas.width - this.size * 2)) + this.size;
    this.y =
      Math.floor(Math.random() * (canvas.width - this.size * 2)) + this.size;
    this.img = new Image();
    this.img.src = "../../assets/image/score.png";
    this.isGreen = Math.random() < 0.1;
    this.imgGreen = new Image();
    this.imgGreen.src = "../../assets/image/greenScore.png";
    if (!this.isGreen) {
      this.isPink = Math.random() < 0.1;
      this.imgPink = new Image();
      this.imgPink.src = "../../assets/image/pinkScore.png";
    }
  }

  draw() {
    if (this.isGreen) {
      this.img = this.imgGreen;
    }
    if (this.isPink) {
      this.img = this.imgPink;
    }
    ctx.drawImage(this.img, this.x, this.y, this.size, this.size);
  }

  notify(x, y, size, enemys) {
    if (collision(x, y, size, this.x, this.y, this.size)) {
      if (this.isGreen) {
        for (let i = enemys.length / 2 - 1; i >= 0; i--) {
          enemys.splice(i, 1);
        }
      }
      if (this.isPink) {
        invincible = true;
        setTimeout(() => {
          invincible = false;
        }, 2000);
      }
      console.log(this.isGreen);
      player.removeObserverScore();
      scoreNum++;
    }
  }

  notifyPriserky(x, y, size) {
    if (
      collision(x + size / 2, y + size / 2, size * 2, this.x, this.y, this.size)
    ) {
      player.removeObserverScore();
    }
  }
}
