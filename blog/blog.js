function main()
{
    var i;
    var colbuts = document.getElementsByClassName("collapse-button");
    for (i = 0; i < colbuts.length; i++)
    {
        colbuts[i].parentElement.nextElementSibling.style.maxHeight = colbuts[i].parentElement.nextElementSibling.scrollHeight + "px";
        colbuts[i].addEventListener("click", 
            function()
            {
                this.classList.toggle("active");
                var content = this.parentElement.nextElementSibling;
                if (!(content.style.maxHeight === "0px"))
                {
                    setTimeout(HideElementsInCollapsedCategory, 200, content);
                    content.style.maxHeight = "0px";
                    this.children[0].src = "blog/collapse-closed.png";
                }
                else
                {
                    content.style.visibility = 'visible';
                    content.style.maxHeight = content.scrollHeight + "px";
                    this.children[0].src = "blog/collapse-open.png";
                }
            }
        );
    }
}

function HideElementsInCollapsedCategory(content)
{
    if (content.style.maxHeight === "0px")
        content.style.visibility = 'hidden';
}

main();