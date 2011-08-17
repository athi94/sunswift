/* Hardware definitions */
#define BIT(x) (1<<x)

#define CLOCK_SPEED 7372800

/* Port 1 */
#define CAN_INT         BIT(7)

/* Port 2 */

/* Port 3 */
#define TX              BIT(4)

/* Port 4 */
#define PWR		BIT(0)

/* Port 5 */
#define TYRE_CS   	BIT(0)
#define SIMO1           BIT(1)
#define SOMI1           BIT(2)
#define UCLK1		BIT(3)
#define LED	        BIT(7)

/* Port 6 / ADC */
#define SOMI2		BIT(3)
#define TEMP_CS		BIT(4)
#define UCLK2		BIT(5)

/* ADC channel definitions */
