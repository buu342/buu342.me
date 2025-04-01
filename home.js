function main()
{
    var date = new Date();
    var icon = document.getElementsByClassName("me-myself-and-i")[0].childNodes[1];
    switch (date.getMonth())
    {
        case 3:
            if (date.getDate() == 1)
                icon.src = "memyselfi_aprilfools.png";
            break;
        case 9:
            icon.src = "memyselfi_october.png";
            break;
        case 11:
            icon.src = "memyselfi_december.png";
            break;
    }
}

main()