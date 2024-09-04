function openNav() {
    const sidenav = document.getElementById("mySidenav");
    const bannerHeight = document.querySelector('.banner').offsetHeight;

    sidenav.style.width = "300px";
    sidenav.classList.add("banner-active");

    document.getElementById("main").style.marginLeft = "225px";
    document.body.style.backgroundColor = "rgba(0,0,0,0.4)";
}

function closeNav() {
    const sidenav = document.getElementById("mySidenav");
    sidenav.style.width = "0";
    sidenav.classList.remove("banner-active");

    document.getElementById("main").style.marginLeft = "0";
    document.body.style.backgroundColor = "white";
}