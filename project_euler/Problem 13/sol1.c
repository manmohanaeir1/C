#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/* Function to read the number from a file and store it in array.
    index 0 of output buffer => units place
    index 1 of output buffer => tens place and so on
    i.e., index i => 10^i th place
 */
int get_number(FILE *fp, char *buffer, uint8_t *out_int)
{
    long l = fscanf(fp, "%s\n", buffer);
    if (!l)
    {
        perror("Error reading line.");
        return -1;
    }
    // printf("Number: %s\t length: %ld, %ld\n", buffer, strlen(buffer), l);

    long L = strlen(buffer);

    for (int i = 0; i < L; i++)
        if (buffer[i] < 0x30 || buffer[i] > 0x39)
        {
            perror("found inavlid character in the number!");
            return -1;
        }
        else
            out_int[L - i - 1] = buffer[i] - 0x30;

    return 0;
}

/**
 * Function to add arbitraty length decimal integers stored in an array.
 * a + b = c = new b
 **/
int add_numbers(uint8_t *a, uint8_t *b, uint8_t N)
{
    int carry = 0;
    uint8_t *c = b; /* accumulate the result in the array 'b' */

    for (int i = 0; i < N; i++)
    {
        // printf("\t%d + %d + %d ", a[i], b[i], carry);
        c[i] = carry + a[i] + b[i];
        if (c[i] > 9) /* check for carry */
        {
            carry = 1;
            c[i] -= 10;
        }
        else
            carry = 0;
        // printf("= %d, %d\n", carry, c[i]);
    }

    for (int i = N; i < N + 10; i++)
    {
        if (carry == 0)
            break;
        // printf("\t0 + %d + %d ", b[i], carry);
        c[i] = carry + c[i];
        if (c[i] > 9)
        {
            carry = 1;
            c[i] -= 10;
        }
        else
            carry = 0;
        // printf("= %d, %d\n", carry, c[i]);
    }
    return 0;
}

int print_number(uint8_t *number, uint8_t N, int8_t num_digits_to_print)
{
    uint8_t start_pos = N - 1;
    uint8_t end_pos;

    /* skip all initial zeros */
    while (number[start_pos] == 0)
        start_pos--;

    /* if end_pos < 0, print all digits */
    if (num_digits_to_print < 0)
        end_pos = 0;
    else if (num_digits_to_print <= start_pos)
        end_pos = start_pos - num_digits_to_print + 1;
    else
    {
        fprintf(stderr, "invalid number of digits argumet!\n");
        return -1;
    }

    for (int i = start_pos; i >= end_pos; i--)
        putchar(number[i] + 0x30);

    putchar('\n');

    return 0;
}

#define N 10
#define N2 (N + 10)

int main(void)
{
    // const char N = 50, N2 = N+10;          /* length of numbers */
    char txt_buffer[N + 5]; /* temporary buffer */
    uint8_t number[N];      /* array to store digits of a large number */
    uint8_t sum[N2];        /* array to store the sum of the large numbers. For
                                safety, we make it twice the length of a number. */

    memset(sum, 0, sizeof(sum)); /* initialize sum array with 0 */

    FILE *fp = fopen("num.txt", "rt"); /* open text file to read */
    if (!fp)
    {
        perror("Unable to open file 'num.txt'.");
        return -1;
    }

    int count = 0;
    get_number(fp, txt_buffer, sum); /* 0 + = first_number = first_number */
    do
    {
        count++;
        if (get_number(fp, txt_buffer, number) != 0)
            break;
        add_numbers(number, sum, N);
    } while (!feof(fp));

    printf("\nSum   : ");
    print_number(sum, N2, -1);

    printf("first 10 digits: \t");
    print_number(sum, N2, 10);

    fclose(fp); /* close file */
    return 0;
}