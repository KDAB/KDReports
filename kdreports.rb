class Kdreports < Formula
  desc "A Qt library for creating printable reports"
  homepage "https://github.com/KDAB/KDReports"
  url "https://build.opensuse.org/source/isv:KDAB/KDReports/kdreports-1.6.0.tar.gz"
  sha256 "b3eb98916eca6a0e9bea31546782d006f66ebb2a5df45f0e68901b13fed20908"

  depends_on "qt"

  def install
    touch ".license.accepted"
    system "./configure.sh", "-shared",
                          "-release",
                          "-qmake", "/usr/local/bin/qmake",
                          "-prefix", "#{prefix}"
    system "make"
    system "make", "install"
  end

  test do
    system "make", "test"
  end
end
