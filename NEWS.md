# unigd 0.1.3

- Added missing standard library include for GCC 15

# unigd 0.1.2

- Fixed an issue that made unigd crash when rendering without any plots in the history on some platforms.
- Update installation instructions (thanks @huangyxi).
- Minor internal improvements.

# unigd 0.1.1

- Fix issues with 'libtiff'. (Thanks @benz0li)
- Update linking on Windows for upcoming version of 'Rtools'. (Thanks @kalibera)

# unigd 0.1.0

- Split graphics rendering and R interface from 'httpgd'.
- Large refactoring and rewrite.
- Add async C client API.
- Add custom inter process communication layer.
- Add TIFF renderer.
- Add Base64 PNG renderer.
- Fix crash when querying capabilities on R 4.2.
- Improve testing.
- Many small fixes and improvements.
