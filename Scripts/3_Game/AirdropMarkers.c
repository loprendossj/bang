modded class DayZGame {
    /* Global Vars */
    private ref array<vector> m_ClientCarePackageMarker;

    void DayZGame() {
        m_ClientCarePackageMarker = new array<vector>;
    }
	
    void ClearCarePackageMarkers() {		
		
		int x = m_ClientCarePackageMarker.Count();
		for( int i=0; i < x; i++ )
		{	
			m_ClientCarePackageMarker.Remove(i);
		}
    }

    vector GetClientCarePackageMarker(int ArrayEntry) {
        return m_ClientCarePackageMarker[ArrayEntry];
    }
	
	int GetClientCarePackageMarkerArrayLength() {
		if (m_ClientCarePackageMarker)
			return m_ClientCarePackageMarker.Count();
		else 
			return 0;
	}
	
	void SetClientCarePackageMarker(vector CarePackage_pos) {
		
		m_ClientCarePackageMarker.Insert(CarePackage_pos);
    }
}

