BEGIN {
  for (i = 1; i < 10; ++i) {
    print i
    if (i > 5) continue
    print i
  }
}
