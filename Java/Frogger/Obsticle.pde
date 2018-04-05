class Obsticle extends Rectangle {
  float speed;
  Obsticle(float x_, float y_, float w_, float speed_, color col_) {
    super(x_, y_, w_, grid.y, col_);
    speed = speed_;
  }

  void update() {
    x = x + speed;
    edges();
  }

  void edges() {
    if ((speed > 0) && (x > width + grid.x + 0.5 * w)) {
      x = - w - grid.x;
    } else if ((speed < 0) && (x < -w - grid.x - 0.5 * w)) {
      x = width + w + grid.x;
    }
  }
}
