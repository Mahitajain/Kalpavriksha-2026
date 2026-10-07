#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#define MAX_EXP_LENGTH 512
#define MAX_INPUT_LENGTH 1024

int precedence(char op)
{
    if (op == '*' || op == '/')
        return 2;
    return 1;
}

int calculate(long long numbers[], char operators[], int *num_top, int *op_top)
{
    if (*op_top < 0 || *num_top < 1)
    {
        printf("Error: Invalid expression.\n");
        return 0;
    }

    char op = operators[*op_top];
    (*op_top)--;

    long long b = numbers[*num_top];
    (*num_top)--;

    long long a = numbers[*num_top];
    (*num_top)--;

    switch (op)
    {
        case '+':
            if ((b > 0 && a > LLONG_MAX - b) || (b < 0 && a < LLONG_MIN - b))
            {
                printf("Error: Integer overflow.\n");
                return 0;
            }
            numbers[++(*num_top)] = a + b;
            break;

        case '-':
            if ((b < 0 && a > LLONG_MAX + b) || (b > 0 && a < LLONG_MIN + b))
            {
                printf("Error: Integer overflow.\n");
                return 0;
            }
            numbers[++(*num_top)] = a - b;
            break;

        case '*':
            if (a > 0)
            {
                if ((b > 0 && a > LLONG_MAX / b) || (b <= 0 && b < LLONG_MIN / a))
                {
                    printf("Error: Integer overflow.\n");
                    return 0;
                }
            }
            else if (a < 0)
            {
                if ((b > 0 && a < LLONG_MIN / b) || (b <= 0 && b < LLONG_MAX / a))
                {
                    printf("Error: Integer overflow.\n");
                    return 0;
                }
            }
            numbers[++(*num_top)] = a * b;
            break;

        case '/':
            if (b == 0)
            {
                printf("Error: Division by zero.\n");
                return 0;
            }
            else
            {
                numbers[++(*num_top)] = a / b;
            }
            break;
    }

    return 1;
}

int evaluate(char *s, long long *answer, long long numbers[], char operators[], int *num_top, int *op_top)
{
    int need_number = 1;

    for (char *p = s; *p; )
    {
        if (isspace((unsigned char)*p))
        {
            p++;
            continue;
        }

        if (need_number)
        {
            if (!isdigit((unsigned char)*p))
            {
                printf("Error: Invalid expression.\n");
                return 0;
            }

            long long number = 0;

            while (isdigit((unsigned char)*p))
            {
                number = number * 10 + (*p - '0');
                p++;
            }

            if (*num_top >= MAX_EXP_LENGTH - 1)
            {
                printf("Error: Invalid expression.\n");
                return 0;
            }

            numbers[++(*num_top)] = number;
            need_number = 0;
        }
        else
        {
            if (*p != '+' && *p != '-' && *p != '*' && *p != '/')
            {
                printf("Error: Invalid expression.\n");
                return 0;
            }

            while (*op_top >= 0 &&
                   precedence(operators[*op_top]) >= precedence(*p))
            {
                if (!calculate(numbers, operators, num_top, op_top))
                    return 0;
            }

            if (*op_top >= MAX_EXP_LENGTH - 1)
            {
                printf("Error: Invalid expression.\n");
                return 0;
            }

            operators[++(*op_top)] = *p;
            p++;
            need_number = 1;
        }
    }

    if (need_number)
    {
        printf("Error: Invalid expression.\n");
        return 0;
    }

    while (*op_top >= 0)
    {
        if (!calculate(numbers, operators, num_top, op_top))
            return 0;
    }

    if (*num_top != 0)
    {
        printf("Error: Invalid expression.\n");
        return 0;
    }

    *answer = numbers[0];
    return 1;
}

int main()
{
    char input[MAX_INPUT_LENGTH];
    long long numbers[MAX_EXP_LENGTH];
    char operators[MAX_EXP_LENGTH];
    int num_top = -1;
    int op_top = -1;

    if (!fgets(input, sizeof(input), stdin))
    {
        printf("Error: Invalid expression.\n");
        return 1;
    }

    input[strcspn(input, "\r\n")] = '\0';

    char *s = input;
    size_t length = strlen(s);

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

    if (!evaluate(s, &answer, numbers, operators, &num_top, &op_top))
        return 1;

    printf("%lld\n", answer);

    return 0;
}