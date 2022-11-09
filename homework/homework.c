#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <termios.h>

#define D1 0x01
#define D2 0x02
#define D3 0x04
#define D4 0x08

static struct termios init_setting, new_setting;
char seg_num[10] = {0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xd8, 0x80, 0x90};
int a = 0, b = 0, c = 0, d = 0;

void init_keyboard();
void close_keyboard();
char get_key();
void print_menu();
void up_count(unsigned short *data);
void down_count(unsigned short *data);

int main(int argc, char **argv)
{
    unsigned short data[4];
    char key;
    int tmp_n;
    int delay_time;

    int dev_segment = open("/dev/my_segment", O_RDWR); // if you want read = 'O_RDONLY' write='O_WRONLY', read&write='O_RDWR'
    int dev_button = open("/dev/my_button", O_RDWR);   // if you want read='O_RDONLY' write='O_WRONLY', read&write='O_RDWR'

    if (dev_segment == -1)
    {
        printf("Opening was not possible! Because of segment!\n");
        return -1;
    }
    if (dev_button == -1)
    {
        printf("Opening was not possible! Because of button!\n");
        return -1;
    }
    printf("device opening was successfull!\n");

    char buff;
    char tmp;
    char prev = 'r';
    int upup = 0, downdown = 0;
    volatile int select = 0;

    init_keyboard();
    print_menu();
    tmp_n = 3;
    delay_time = 1000;

    data[0] = (seg_num[a] << 4) | D1; // first diode
    data[1] = (seg_num[b] << 4) | D2; // second diode
    data[2] = (seg_num[c] << 4) | D3; // third diode
    data[3] = (seg_num[d] << 4) | D4; // fourth diode

    while (1)
    {
        key = get_key();

        if (key == 'q')
        {
            printf("exit this program.\n");
            break;
        }

        else if (key == 'p')
        {
            printf("RESET\n");
            a = 0, b = 0, c = 0, d = 0;
            data[0] = (seg_num[a] << 4) | D1; // first diode
            data[1] = (seg_num[b] << 4) | D2; // second diode
            data[2] = (seg_num[c] << 4) | D3; // third diode
            data[3] = (seg_num[d] << 4) | D4; // fourth diode
            tmp_n = 3;
        }

        else if (key == 'u')
        {
            printf("UP:keyboard\n");
            up_count(data);
            tmp_n = 3;
        }

        else if (key == 'd')
        {
            printf("DOWN:keyboard\n");
            down_count(data);
            tmp_n = 3;
        }

        select = read(dev_button, &buff, 1); // up or down
        prev = tmp;
        tmp = buff;

        if (prev != tmp)
        {
            upup++;
            downdown++;

            if (select == 1) // up button
            {
                printf("UP:button\n");
                up_count(data);
                /* avoid chattering */
                upup = 0;
                downdown = 0;
            }

            else if (select == 2) // down button
            {
                printf("DOWN:button\n");
                down_count(data);
                /* avoid chattering */
                upup = 0;
                downdown = 0;
            }
        }

        write(dev_segment, &data[tmp_n], 2);
        usleep(delay_time);
        tmp_n--;

        if (tmp_n < 0)
        {
            tmp_n = 3;
        }
    }

    close_keyboard();
    write(dev_segment, 0x0000, 2);
    close(dev_segment);
    close(dev_button);
    return 0;
}

void init_keyboard()
{
    tcgetattr(STDIN_FILENO, &init_setting);
    new_setting = init_setting;
    new_setting.c_lflag &= ~ICANON;
    new_setting.c_lflag &= ~ECHO;
    new_setting.c_cc[VMIN] = 0;
    new_setting.c_cc[VTIME] = 0;
    tcsetattr(0, TCSANOW, &new_setting);
}

void close_keyboard()
{
    tcsetattr(0, TCSANOW, &init_setting);
}

char get_key()
{
    char ch = -1;

    if (read(STDIN_FILENO, &ch, 1) != 1)
        ch = -1;
    return ch;
}

void print_menu()
{
    printf("\n+---------menu---------+\n");
    printf("| [u] : count up       |\n");
    printf("| [d] : count down     |\n");
    printf("| [p] : count reset    |\n");
    printf("| [q] : program quit   |\n");
    printf("+----------------------+\n\n");
}

void up_count(unsigned short *data)
{
    if (d == 9) // fourth diode
    {
        d = 0;
        data[3] = (seg_num[d] << 4) | D4;
        if (c == 9) // third diode
        {
            c = 0;
            data[2] = (seg_num[c] << 4) | D3;
            if (b == 9) // second diode
            {
                b = 0;
                data[1] = (seg_num[b] << 4) | D2;
                if (a == 9) // first diode
                {
                    a = 0;
                    data[0] = (seg_num[a] << 4) | D1;
                    return;
                }
                a++;
                data[0] = (seg_num[a] << 4) | D1;
                return;
            }
            b++;
            data[1] = (seg_num[b] << 4) | D2;
            return;
        }
        c++;
        data[2] = (seg_num[c] << 4) | D3;
        return;
    }
    d++;
    data[3] = (seg_num[d] << 4) | D4;
    return;
}

void down_count(unsigned short *data)
{
    if (d == 0) // fourth diode
    {
        d = 9;
        data[3] = (seg_num[d] << 4) | D4;
        if (c == 0) // third diode
        {
            c = 9;
            data[2] = (seg_num[c] << 4) | D3;
            if (b == 0) // second diode
            {
                b = 9;
                data[1] = (seg_num[b] << 4) | D2;
                if (a == 0) // first diode
                {
                    a = 9;
                    data[0] = (seg_num[a] << 4) | D1;
                    return;
                }
                a--;
                data[0] = (seg_num[a] << 4) | D1;
                return;
            }
            b--;
            data[1] = (seg_num[b] << 4) | D2;
            return;
        }
        c--;
        data[2] = (seg_num[c] << 4) | D3;
        return;
    }
    d--;
    data[3] = (seg_num[d] << 4) | D4;
    return;
}

