
#pragma once


namespace CoreStructures {

	// structure to model library and type version meta-data
	struct GUVersion {

	private:

		short		majorVersion, minorVersion, rev;

	public:

		GUVersion(const short _major, const short _minor, const short _rev) {

			majorVersion = _major;
			minorVersion = _minor;
			rev = _rev;
		}

		short getMajorVersion() const {

			return majorVersion;
		}

		short getMinorVersion() const {

			return minorVersion;
		}

		short getRevision() const {

			return rev;
		}
	};

}
