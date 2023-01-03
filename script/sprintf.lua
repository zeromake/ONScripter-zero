for i = 0, 32 do
    printf("case %d:\n    sprintf(buff, format",  i)
    for j = 0, i - 1 do
        printf(", args[%d]", j)
    end
    printf(");\n    break;\n")
end
