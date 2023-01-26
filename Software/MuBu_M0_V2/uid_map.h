/*
 * The UID_MAP is used to convert RFID chip UIDs into integer based IDs.
 * Each UID is associated with a unique ID (manually selected).
 * 
 * The get functions allows for a straightforward vonversion from UID to ID.
 */

struct UID_ENTRY {
  uint32_t uid;
  uint8_t id;  
};

struct UID_MAP {
  // don't forget to change the size constant if more UIDs are added
  static const uint8_t size = 7;
  UID_ENTRY map [size] = {
    {1826630060,1},
    {1841400410,2},
    {1941915971,3},
    {1936862019,4},
    {57815107,5},
    {4088808515,6},
    {3664388502,7}
  };

  /* 
   * converts passed uid into id 
   */
  int get (uint32_t uid) {
    for (int i=0; i<size; i++) {
      if (map[i].uid==uid) return map[i].id;
    }
    return -1;
  }
};

UID_MAP uid_map;
