class Enemy {
  constructor() {
    this.enemySpawn = Math.floor(Math.random() * player.priserky.length);
    this.x = player.priserky[this.enemySpawn].x;
    this.y = player.priserky[this.enemySpawn].y;
    this.size = level.enemy_size[levelSelected];
    this.img = new Image();
    this.img.src = "../../assets/image/enemy.png";
    this.speed = level.enemy_speed[levelSelected];
    this.dx =
      (Math.random() * (this.speed - this.speed / 2) + this.speed / 2) *
      (Math.random() < 0.5 ? -1 : 1);
    this.dy =
      (Math.random() * (this.speed - this.speed / 2) + this.speed / 2) *
      (Math.random() < 0.5 ? -1 : 1);
  }

  draw() {
    ctx.drawImage(this.img, this.x, this.y, this.size, this.size);
    this.update();
  }

  update() {
    this.x += this.dx;
    this.y += this.dy;

    if (this.x < 0 || this.x + this.size > canvas.width) {
      this.dx = -this.dx;
    }
    if (this.y < 0 || this.y + this.size > canvas.height) {
      this.dy = -this.dy;
    }
  }

  notify(x, y, size) {
    if (!invincible) {
      if (collision(x, y, size, this.x, this.y, this.size)) {
        OverFunction();
      }
    }
  }
}
