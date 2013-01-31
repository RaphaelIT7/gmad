
#ifndef ADDON_JSON_H
#define ADDON_JSON_H

class CAddonJson
{
	public:

		CAddonJson( Bootil::BString strInfoFile )
		{
			Bootil::BString strFileContents;

			//
			// Try to open the file
			//
			if ( !Bootil::File::Read( strInfoFile, strFileContents ) )
			{
				m_strError = "Couldn't find file";
				return;
			}

			Bootil::Data::Tree tree;

			//
			// Parse the JSON
			//
			if ( !Bootil::Data::Json::Import( tree, strFileContents ) )
			{
				m_strError = "Couldn't parse json";
				return;
			}

			//
			// Check the title
			//
			m_Title		= tree.ChildValue( "title" );
			if ( m_Title.empty() )
			{
				m_strError = "title is empty!";
				return;
			}

			//
			// Get the description
			//
			m_Description	= tree.ChildValue( "description", "Description" );

			//
			// Load the addon type
			//
			m_AddonType		= tree.ChildValue( "type", "" );
			{
				Bootil::String::Lower( m_AddonType );

				if ( m_AddonType.empty() )
				{
					m_strError = "type is empty!";
					return;
				}

				//
				// Verify that the addon type is valid by checking it against the list of valids
				//
				for ( int i=0;;i++ )
				{
					if ( Addon::Tags::Type[i] == NULL )
					{
						m_strError = "type isn't a supported type!";
						return;
					}

					if ( m_AddonType == Addon::Tags::Type[i] )
						break;
				}
			}
			

			//
			// Parse the ignores
			//
			Bootil::Data::Tree ignores = tree.GetChild( "ignore" );
			BOOTIL_FOREACH( child, ignores.Children(), Bootil::Data::Tree::List )
			{
				m_Ignores.push_back( child->Value() );
			}

		}

		void RemoveIgnoredFiles( Bootil::String::List& files )
		{
			Bootil::String::List old_files = files;
			files.clear();

			BOOTIL_FOREACH( f, old_files, Bootil::String::List )
			{
				bool bSkipFile = false;

				//
				// Never include our json file!
				//
				if ( *f == "addon.json" ) continue;

				//
				// Check against our loaded ignores list
				//
				BOOTIL_FOREACH( ignore, m_Ignores, Bootil::String::List )
				{
					if ( Bootil::String::Test::Wildcard( *ignore, *f ) )
					{
						bSkipFile = true;
						break;
					}
				}

				if ( !bSkipFile )
					files.push_back( *f );
			}
		}

		const Bootil::BString& GetError(){ return m_strError; }
		const Bootil::BString& GetTitle(){ return m_Title; }
		const Bootil::BString& GetDescription(){ return m_Description; }
		const Bootil::BString& GetType(){ return m_AddonType; }
		

		Bootil::BString			m_strError;

		Bootil::BString			m_Title;
		Bootil::BString			m_Description;
		Bootil::BString			m_AddonType;
		Bootil::String::List	m_Ignores;
};



#endif