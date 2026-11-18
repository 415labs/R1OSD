const char* itoaPadding(int val, int padding, char paddingCharacter)
{
  if(padding > 29)
    padding = 29;
  static char buf[30];
  char* ptr = buf+29;
  int neg = (val < 0);
  if(neg) {
    val = -val;
    --padding;
  }
  *ptr-- = 0;
  do {
    *ptr-- = (val % 10) + '0';
    val /= 10;
    --padding;
  } while(val);
  for(; padding > 0; --padding)
    *ptr-- = paddingCharacter;	// Si on veut padder avec des zeros, mettre '0' ici.
  if(neg)
    *ptr-- = '-';
  return ++ptr;
}
