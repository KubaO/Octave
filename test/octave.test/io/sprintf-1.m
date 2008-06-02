[s, msg, status] = sprintf ("%s: %d\n", "test", 1);

s == "test: 1\n" && ischar (msg) && status == 8
