# EndraPDF Lite Run001

PDF-only companion for EndraHome Run049. Derived from Legimet nPDF v0.4b
(commit ba05f334b6500170c6cb74099d4cdfc050a69e36). The patch registers
only the PDF handler, removes extension registration on standalone launch,
handles open failures and updates the SDK millisecond sleep call.
Size optimization and linker garbage collection omit unused code. PDF image
decoders and the base PDF fonts remain available.

To compare on hardware, keep the original nPDF.tns with the working desktop.
Copy the Run049 desktop into a separate calculator folder and put the Lite
file there under the exact name nPDF.tns. Launch that copy of EndraHome.
Open a text PDF and a scanned PDF, change pages, zoom, then Esc to Files.
The original folder remains the rollback. For normal use only one desktop
and one reader are needed.

The workflow runs prepare.sh in a clean directory, host.sh on Linux and
native.sh in the pinned Ndless SDK container. The host regression checks
both pages for visible rendered pixels. The compiler must tolerate the old
renderer accessing rectangle coordinates through point pointers. Upstream
copyright and license texts are kept in the source distribution.
