#define BUTTON_RIGHT 2
#define BUTTON_UP 4
#define BUTTON_DOWN 5
#define BUTTON_X 9
#define BUTTON_A 7

#include <stdint.h>

class KEYPAD
{
  public:
  KEYPAD();  
  bool keylock;
};

class BUTTONS {
    private: 
    uint16_t button_state;
    volatile uint8_t flag_rising;
    volatile uint8_t flag_falling;
    volatile uint8_t button_prev_state;
    volatile uint8_t button_new_state;
    uint GPIO_id;
    //void Init();
    public:
    BUTTONS(uint GPIO);
    void update(void);
    bool get_direct();
    uint8_t get();
    uint8_t rising_edge();
    uint8_t falling_edge();

};

