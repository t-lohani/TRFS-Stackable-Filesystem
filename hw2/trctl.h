#define MYDEV_MAGIC 'G'
#define ALL  _IOW(MYDEV_MAGIC, 0, unsigned long)
#define NONE  _IOW(MYDEV_MAGIC, 1, unsigned long)
#define HEX  _IOW(MYDEV_MAGIC, 2, unsigned long)
#define RET _IOR(MYDEV_MAGIC, 3, unsigned long *)
