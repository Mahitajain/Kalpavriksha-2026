#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX 512

long long numbers[MAX];
char operators[MAX];
int num_top = -1;
int op_top = -1;
int divide_by_zero = 0;

int precedence(char op)
{
    if (op == '*' || op == '/')
        return 2;
    return 1;
}

int calculate()
{
    if (op_top < 0 || num_top < 1)
        return 0;

    char op = operators[op_top];
    op_top--;

    long long b = numbers[num_top];
    num_top--;

    long long a = numbers[num_top];
    num_top--;  

    switch (op)
    {
        case '+':
            numbers[++num_top] = a + b;
            break;

        case '-':
            numbers[++num_top] = a - b;
            break;

        case '*':
            numbers[++num_top] = a * b;
            break;

        case '/':
            if (b == 0)
            {
                divide_by_zero = 1;
                numbers[++num_top] = 0;
            }
            else
            {
                numbers[++num_top] = a / b;
            }
            break;
    }

    return 1;
}

int evaluate(char *s, long long *answer)
{
    int need_number = 1;
    int sign = 1;

    for (char *p = s; *p; )
    {
        if (isspace((unsigned char)*p))
        {
            p++;
            continue;
        }

        if (need_number)
        {
            if (*p == '-')
            {
                sign = -sign;
                p++;
                continue;
            }

            if (*p == '+')
            {
                p++;
                continue;
            }

            if (!isdigit((unsigned char)*p))
                return 0;

            long long number = 0;

            while (isdigit((unsigned char)*p))
            {
                number = number * 10 + (*p - '0');
                p++;
            }

            if (num_top >= MAX - 1)
                return 0;

            numbers[++num_top] = sign * number;
            sign = 1;
            need_number = 0;
        }
        else
        {
            if (*p != '+' && *p != '-' && *p != '*' && *p != '/')
                return 0;

            while (op_top >= 0 &&
                   precedence(operators[op_top]) >= precedence(*p))
            {
                if (!calculate())
                    return 0;
            }

            if (op_top >= MAX - 1)
                return 0;

            operators[++op_top] = *p;
            p++;
            need_number = 1;
        }
    }

    if (need_number)
        return 0;

    while (op_top >= 0)
    {
        if (!calculate())
            return 0;
    }

    if (num_top != 0)
        return 0;

    *answer = numbers[0];
    return 1;
}

int main()
{
    char input[1024];

    if (!fgets(input, sizeof(input), stdin))
    {
        printf("Error: Invalid expression.\n");
        return 1;
    }

    input[strcspn(input, "\r\n")] = '\0';

    char *s = input;
    int length = strlen(s);

    if (length >= 2 && s[0] == '"' && s[length - 1] == '"')
    {
        s[length - 1] = '\0';
        s++;
    }

    for (char *p = s; *p; p++)
    {
        if (!isdigit((unsigned char)*p) &&
            !isspace((unsigned char)*p) &&
            !strchr("+-*/", *p))
        {
            printf("Error: Invalid expression.\n");
            return 1;
        }
    }

    long long answer;

    if (!evaluate(s, &answer))
    {
        printf("Error: Invalid expression.\n");
        return 1;
    }

    if (divide_by_zero)
    {
        printf("Error: Division by zero.\n");
        return 1;
    }

    printf("%lld\n", answer);

    return 0;
}