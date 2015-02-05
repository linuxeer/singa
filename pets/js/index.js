$(document).ready(
    function () {
        $.ajax({
            url: 'getPets.do',
            data: {
                name: 'cat',
                age: 1
            },
            dataType: 'json',
            success: function (resp) {
                console.log(typeof resp);
                console.log(resp.format);
            }
        });
    }
);
