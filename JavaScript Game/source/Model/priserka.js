class Priserka {
  constructor() {
    this.size = 50;
    this.animPos = 0;
    this.x =
      Math.floor(Math.random() * (canvas.width - this.size * 2)) + this.size;
    this.y =
      Math.floor(Math.random() * (canvas.width - this.size * 2)) + this.size;
    this.img = new Image();
    this.img.src = "../../assets/image/priserka.png";
  }

  draw() {
    ctx.drawImage(
      this.img,
      this.size * this.animPos,
      0,
      this.size,
      this.size,
      this.x,
      this.y,
      this.size,
      this.size
    );
    this.currentPosition();
  }

  notify(x, y, size) {
    if (!invincible) {
      if (collision(x, y, size, this.x, this.y, this.size)) {
        OverFunction();
      }
    }
  }

  animation() {
    this.animPos = 1;
    setTimeout(1000);
    this.animPos = 2;
    setTimeout(1000);
    this.animPos = 0;
  }

  currentPosition() {
    player.scores.forEach((score) => {
      player.priserky.forEach((priserka) => {
        score.notifyPriserky(priserka.x, priserka.y, priserka.size);
      });
    });
  }
}
