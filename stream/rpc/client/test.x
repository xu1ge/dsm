struct para {
  char* buff;
  int len;
}
program TESTPROG {
   version VERSION {
     int WRITE_DATA(para) = 1;
     int READ_DATA(para) = 2;
   } = 1;
} = 87654321;
