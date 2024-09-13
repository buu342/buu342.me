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
                    setTimeout(HideElements, 200, content);
                    content.style.maxHeight = "0px";
                    this.children[0].src = "projects/collapse-closed.png";
                }
                else
                {
                    content.style.visibility = 'visible';
                    content.style.maxHeight = content.scrollHeight + "px";
                    this.children[0].src = "projects/collapse-open.png";
                }
            }
        );
    }

    var carouselobject = document.getElementsByClassName("carousel-object")[0];
    if (carouselobject != null)
    {
        var carouselimage = carouselobject.children[0];
        var projimgbuts = document.getElementsByClassName("carousel-list-object");
        for (i = 0; i < projimgbuts.length; i++)
        {
            projimgbuts[i].addEventListener("click",
                function()
                {
                    var oldselected = document.getElementsByClassName("carousel-list-object selected")[0];
                    if (oldselected == this)
                        return;

                    oldselected.classList.remove("selected")
                    carouselimage.classList.remove("fade")
                    carouselimage.offsetWidth
                    carouselimage.classList.add("fade")
                    setTimeout(ChangeImage, 250, carouselimage, this.id);
                    this.classList.add("selected")
                }
            );
        }
    }
}

function HideElements(content)
{
    if (content.style.maxHeight === "0px")
        content.style.visibility = 'hidden';
}

function ChangeImage(content, src)
{
    content.src = src;
}

main();