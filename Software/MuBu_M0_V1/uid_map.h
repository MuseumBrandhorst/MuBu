struct ENTRY {
  uint32_t uid;
  uint8_t id;  
};

struct UID_MAP {
  int size = 7;
  ENTRY map [7] = {
    {1826630060,1},
    {1841400410,2},
    {1941915971,3},
    {1936862019,4},
    {57815107,5},
    {4088808515,6},
    {3664388502,7}
  };

  int get (uint32_t uid) {
    for (int i=0; i<size; i++) {
      if (map[i].uid==uid) return map[i].id;
    }
    return -1;
  }
};

UID_MAP uid_map;
