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
        var carouselvideo = carouselobject.children[1];
        var projobjbuts = document.getElementsByClassName("carousel-list-object");
        for (i = 0; i < projobjbuts.length; i++)
        {
            projobjbuts[i].addEventListener("click",
                function()
                {
                    var oldselected = document.getElementsByClassName("carousel-list-object selected")[0];
                    if (oldselected == this)
                        return;
                    if (this.classList.contains("img"))
                    {
                        oldselected.classList.remove("selected");
                        if (canfadeout)
                        {
                            FadeObject(carouselimage);
                            FadeObject(carouselvideo);
                        }
                        this.classList.add("selected")
                        setTimeout(ChangeImage, 250, carouselimage, carouselvideo, this.id);
                    }
                    else
                    {
                        oldselected.classList.remove("selected")
                        if (canfadeout)
                        {
                            FadeObject(carouselimage);
                            FadeObject(carouselvideo);
                        }
                        this.classList.add("selected")
                        setTimeout(ChangeVideo, 250, carouselimage, carouselvideo, this.id);
                    }
                }
            );
        }
    }
}

function HideElementsInCollapsedCategory(content)
{
    if (content.style.maxHeight === "0px")
        content.style.visibility = 'hidden';
}

function ChangeImage(image, video, src)
{
    SetObjectVisiblity(video, false);
    image.src = src;
    setTimeout(FadeObject, 250, image, true);
}

function ChangeVideo(image, video, src)
{
    SetObjectVisiblity(image, false);
    video.src = "https://www.youtube.com/embed/" + src;
    setTimeout(FadeObject, 250, video, true);
}

function FadeObject(content, fadein)
{
    if (fadein)
    {
        content.classList.remove("fadeout");
        content.classList.remove("fadein");
        content.offsetWidth;
        content.classList.add("fadein");
        SetObjectVisiblity(content, true);
    }
    else
    {
        content.classList.remove("fadein");
        content.classList.remove("fadeout");
        content.offsetWidth;
        content.classList.add("fadeout");
        setTimeout(SetObjectVisiblity, 200, content, false);
    }
}

function SetObjectVisiblity(content, visible)
{
    if (visible)
        content.style.display = 'block';
    else
        content.style.display = 'none';
}

main();