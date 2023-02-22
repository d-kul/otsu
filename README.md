# otsu
Код к четвёртому домашнему заданию курса Архитектуры ЭВМ у2022 КТ ИТМО. Программа считает порог значений изображения в формате .pgm, используя [метод Оцу](https://ru.wikipedia.org/wiki/%D0%9C%D0%B5%D1%82%D0%BE%D0%B4_%D0%9E%D1%86%D1%83), обрабатывая изображение параллельно на процессоре с использованием API OpenMP и сохраняет изображение, обработанное порогом. [otsu_one.cpp](src/otsu_one.cpp) --- код, считающий один порог, [otsu.cpp](src/otsu.cpp) --- код, считающий три порога (метод мульти-Оцу).

Скрипт [convert_gif.sh](convert_gif.sh) проводит пороговую обработку файла .gif по кадрам и пересобирает обработанные изображения. Также для обработки gif-изображений трбуются пакеты **ffmpeg**, **libavformat**, **imagemagick**, **gcc** с поддержкой **openmp**.
## Формат ввода
`./otsu [n] [input image] [output image]` где n --- количество потоков (0 --- значение по умолчанию, -1 --- работа без OpenMP), а `./otsu` - исполняемый фаил, скомпилированный от исходника кода (с флагом `-fopenmp`)

`./convert_gif.sh [.cpp source file] [gif file]`
## Примеры
#### pgm
<img src="img/in.jpg" alt="img1" width="30%"> <img src="img/mono/in_new.jpg" alt="img1-mono" width="30%"> <img src="img/multi/in_new.jpg" alt="img1-multi" width="30%">
<img src="img/stars.jpg" alt="img2" width="30%"> <img src="img/mono/stars_new.jpg" alt="img2-mono" width="30%"> <img src="img/multi/stars_new.jpg" alt="img2-multi" width="30%">
<img src="img/monaLisa.jpg" alt="img2" width="30%"> <img src="img/mono/monaLisa_new.jpg" alt="img2-mono" width="30%"> <img src="img/multi/monaLisa_new.jpg" alt="img2-multi" width="30%">
#### gif
<img src="gif/teapot.gif" alt="gif1" width="30%"> <img src="gif/mono/teapot_new.gif" alt="gif1-mono" width="30%"> <img src="gif/multi/teapot_new.gif" alt="gif1-multi" width="30%">
<img src="gif/drop.gif" alt="gif2" width="30%"> <img src="gif/mono/drop_new.gif" alt="gif2-mono" width="30%"> <img src="gif/multi/drop_new.gif" alt="gif2-multi" width="30%">
