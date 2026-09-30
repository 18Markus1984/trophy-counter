@echo off
rem Downloads the photos from the Printables page into docs\images\build\
rem Double-click it or run it from any folder: fetch_images.bat
setlocal
if not exist "%~dp0docs\images\build" mkdir "%~dp0docs\images\build"
cd /d "%~dp0docs\images\build"

echo Gallery
echo   gallery-1.png
curl -sSfL -A "Mozilla/5.0" -o "gallery-1.png" "https://media.printables.com/media/prints/a265fcfa-4029-40ff-b416-c47c47683572/images/13422949_e904c4da-de98-4399-b3e3-0a3a994c3f9f_0b8adf04-3daf-416d-955b-cba67de21f2a/thumbs/cover/800x800/png/printablesmakerworld-3d-tetris24.png" || echo     FAILED: gallery-1.png
echo   gallery-2.png
curl -sSfL -A "Mozilla/5.0" -o "gallery-2.png" "https://media.printables.com/media/prints/df85ca6e-2086-47f2-9845-45f864b7eb83/images/13419475_69f7400f-2715-4ba1-a5f1-3b9e3229969f_e60b047d-8c7e-4645-968d-1b4b88679258/thumbs/cover/800x800/png/printablesmakerworld-3d-tetris17.png" || echo     FAILED: gallery-2.png
echo   gallery-3.jpg
curl -sSfL -A "Mozilla/5.0" -o "gallery-3.jpg" "https://media.printables.com/media/prints/dcb3cce3-1932-45a2-9593-d2fadf164172/images/13419388_36c2c9e7-607c-4275-be92-b9970dc49959_09ebac4b-11d3-4604-bc98-14efc31d21f3/thumbs/cover/800x800/jpg/img20260718222734.jpg" || echo     FAILED: gallery-3.jpg

echo Original version
echo   original-1.webp
curl -sSfL -A "Mozilla/5.0" -o "original-1.webp" "https://media.printables.com/media/prints/1790108/rich_content/30de17d8-fce1-48d3-a371-791b75b2490c/thumbs/cover/800x1067/jpg/img20260718214047.webp" || echo     FAILED: original-1.webp
echo   original-2.webp
curl -sSfL -A "Mozilla/5.0" -o "original-2.webp" "https://media.printables.com/media/prints/1790108/rich_content/c18a19bc-68e7-475a-a232-44fbf2a2101a/thumbs/cover/800x1067/jpg/img20260718214054.webp" || echo     FAILED: original-2.webp

echo Making
echo   making-1.webp
curl -sSfL -A "Mozilla/5.0" -o "making-1.webp" "https://media.printables.com/media/prints/1790108/rich_content/d265bd89-eda5-4269-8daf-e432c524f707/thumbs/cover/800x600/jpg/img20260718222734.webp" || echo     FAILED: making-1.webp
echo   making-2.webp
curl -sSfL -A "Mozilla/5.0" -o "making-2.webp" "https://media.printables.com/media/prints/1790108/rich_content/505a02ca-fa52-4362-9570-4ef9ce18411e/thumbs/cover/800x1067/jpg/img20260718225203.webp" || echo     FAILED: making-2.webp
echo   making-3.png
curl -sSfL -A "Mozilla/5.0" -o "making-3.png" "https://media.printables.com/media/prints/1790108/rich_content/3759ee22-2874-4571-a31a-e3d73205ec60/grafik.png" || echo     FAILED: making-3.png

echo Soldering
echo   soldering.webp
curl -sSfL -A "Mozilla/5.0" -o "soldering.webp" "https://media.printables.com/media/prints/1790108/rich_content/95e35a33-0dcb-47e4-af05-6fe5af160a5f/thumbs/cover/800x364/png/youtubeprintablescounter.webp" || echo     FAILED: soldering.webp

echo.
echo Done. Files are in %CD%
pause
