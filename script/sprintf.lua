printf('case 0:\n    snprintf(buff, size, "%s", format);\n    break;\n')
for i = 1, 32 do
    printf("case %d:\n    snprintf(buff, size, format",  i)
    for j = 0, i - 1 do
        printf(", args[%d]", j)
    end
    printf(");\n    break;\n")
end
