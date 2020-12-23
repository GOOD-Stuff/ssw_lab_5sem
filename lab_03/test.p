program lab;
    var
        a,b,k: integer = 1;
        metka : label;
    begin
    metka:
    if a = k then goto metka else k := 100+ 123;
    b := 100;
    goto metka;
    end.