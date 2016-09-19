BEGIN { }
END   { }
1     { }
/abc/ { print $1 }
1 && 2 { }
/abc/,/def/ { }
