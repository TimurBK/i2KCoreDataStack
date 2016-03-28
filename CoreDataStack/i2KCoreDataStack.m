//
//  i2KCoreDataStack.m
//  i2KCoreDataStack
//
//  Created by Timur Kuchkarov on 28.02.16.
//  Copyright © 2016 Timur Kuchkarov. All rights reserved.
//

#import "i2KCoreDataStack.h"

@interface i2KCoreDataStack ()

@property (nonatomic, strong, readwrite) NSManagedObjectContext *mainManagedObjectContext;
@property (nonatomic, strong, readwrite) NSManagedObjectContext *dataExportManagedObjectContext;
@property (nonatomic, strong, readwrite) NSManagedObjectContext *importManagedObjectContext;

@property (nonatomic, strong) NSURL *modelURL;
@property (nonatomic, strong) NSURL *storeURL;
@property (nonatomic, assign) i2KCDPStoreType storeType;
@property (nonatomic, strong) NSDictionary *persistenceOptions;

@property (nonatomic, strong) NSString *cdStoreType;
@property (nonatomic, strong) NSManagedObjectModel *model;
@property (nonatomic, strong) NSPersistentStoreCoordinator *defaultCoordinator;

@end

@implementation i2KCoreDataStack

- (instancetype)init
{
	return [self initWithAutoMigratingSQLiteStoreWithModelName:@"Model.sqlite"];
}

- (instancetype)initWithStoreType:(i2KCDPStoreType)storeType storeURL:(NSURL * _Nullable)storeURL modelURL:(NSURL *)modelURL options:(NSDictionary *)options
{
	self = [super init];
	if (self) {
		_modelURL = modelURL;
		_storeURL = storeURL;
		_storeType = storeType;
		_persistenceOptions = options;
		[self setupPersistence];
	}
	return self;
}

- (instancetype)initWithAutoMigratingSQLiteStoreWithModelName:(NSString *)modelName
{
	NSURL* documentsDirectory = [[NSFileManager defaultManager] URLForDirectory:NSDocumentDirectory
																	   inDomain:NSUserDomainMask
															  appropriateForURL:nil
																		 create:YES
																		  error:NULL];
	NSURL *storeURL = [documentsDirectory URLByAppendingPathComponent:modelName];
	NSString *fileName = [modelName stringByDeletingPathExtension];
	NSURL *modelURL = [[NSBundle mainBundle] URLForResource:fileName withExtension:@"momd"];

	NSDictionary *automigratingOptions = @{
										   NSInferMappingModelAutomaticallyOption:@YES,NSMigratePersistentStoresAutomaticallyOption:@YES
										   };
	return [self initWithStoreType:i2KCDPStoreTypeSQLite storeURL:storeURL modelURL:modelURL options:automigratingOptions];
}

- (instancetype)initWithAutoMigratingInMemoryStoreWithModelName:(NSString *)modelName
{
	NSURL* documentsDirectory = [[NSFileManager defaultManager] URLForDirectory:NSDocumentDirectory
																	   inDomain:NSUserDomainMask
															  appropriateForURL:nil
																		 create:YES
																		  error:NULL];
	NSURL *storeURL = [documentsDirectory URLByAppendingPathComponent:modelName];
	NSString *fileName = [modelName stringByDeletingPathExtension];
	NSURL *modelURL = [[NSBundle mainBundle] URLForResource:fileName withExtension:@"momd"];

	NSDictionary *automigratingOptions = @{
										   NSInferMappingModelAutomaticallyOption:@YES,NSMigratePersistentStoresAutomaticallyOption:@YES
										   };
	return [self initWithStoreType:i2KCDPStoreTypeInMemory storeURL:storeURL modelURL:modelURL options:automigratingOptions];
}

- (NSManagedObjectContext *)contextWithParent:(NSManagedObjectContext * _Nullable)parentContext concurrencyType:(NSManagedObjectContextConcurrencyType)concurrencyType persistentStoreCoordinator:(NSPersistentStoreCoordinator *)coordinator
{
	NSManagedObjectContext *context = [[NSManagedObjectContext alloc] initWithConcurrencyType:concurrencyType];
	if (parentContext != nil) {
		context.parentContext = parentContext;
	}
	context.persistentStoreCoordinator = coordinator;
	context.mergePolicy = NSMergeByPropertyObjectTrumpMergePolicy;
	context.undoManager = nil;

	return context;
}

- (NSManagedObjectContext *)createPrivateQueueContext
{
	return [self contextWithParent:self.mainManagedObjectContext concurrencyType:NSPrivateQueueConcurrencyType persistentStoreCoordinator:self.defaultCoordinator];
}

- (NSManagedObjectContext *)createMainQueueContext
{
	return [self contextWithParent:self.mainManagedObjectContext concurrencyType:NSMainQueueConcurrencyType persistentStoreCoordinator:self.defaultCoordinator];
}

- (NSManagedObjectContext *)createBackgroundImportContext
{
	NSPersistentStoreCoordinator *coordinator = [self createPersistentStoreCoordinatorWithStoreType:NSSQLiteStoreType model:self.model storeURL:self.storeURL options:self.persistenceOptions];
	return [self contextWithParent:nil concurrencyType:NSPrivateQueueConcurrencyType persistentStoreCoordinator:coordinator];
}

#pragma mark - Internal

- (void)setupPersistence
{
	switch (self.storeType) {
		case i2KCDPStoreTypeInMemory: {
			self.cdStoreType = NSInMemoryStoreType;
			break;
		}
		case i2KCDPStoreTypeSQLite: {
			self.cdStoreType = NSSQLiteStoreType;
			break;
		}
	}

	self.model = [[NSManagedObjectModel alloc] initWithContentsOfURL:self.modelURL];

	self.defaultCoordinator = [self createPersistentStoreCoordinatorWithStoreType:self.cdStoreType model:self.model storeURL:self.storeURL options:self.persistenceOptions];

	self.mainManagedObjectContext = [self contextWithParent:nil concurrencyType:NSMainQueueConcurrencyType persistentStoreCoordinator:self.defaultCoordinator];
	self.importManagedObjectContext = [self createBackgroundImportContext];
	self.dataExportManagedObjectContext = [self createPrivateQueueContext];

	NSAssert(self.mainManagedObjectContext != nil, @"Failed to create main managed object context");
	NSAssert(self.importManagedObjectContext != nil, @"Failed to create import managed object context");
	NSAssert(self.dataExportManagedObjectContext != nil, @"Failed to create export managed object context");
}

- (NSPersistentStoreCoordinator *)createPersistentStoreCoordinatorWithStoreType:(NSString *)storeType model:(NSManagedObjectModel *)model storeURL:(NSURL *)storeURL options:(NSDictionary *)options
{
	NSAssert(model != nil, @"MOModel is nil");

	// Create persistent store coordinator
	NSPersistentStoreCoordinator *persistentStoreCoordinator = [[NSPersistentStoreCoordinator alloc] initWithManagedObjectModel:self.model];
	NSAssert(persistentStoreCoordinator != nil, @"Persistent store failed to create");

	NSError *persistentStoreError = nil;
	NSPersistentStore *persistentStore = [persistentStoreCoordinator addPersistentStoreWithType:storeType
																				  configuration:nil
																							URL:storeURL
																						options:options
																						  error:&persistentStoreError];
	BOOL isMigrationError = ((persistentStoreError.code == NSPersistentStoreIncompatibleVersionHashError) || (persistentStoreError.code == NSMigrationMissingSourceModelError) || (persistentStoreError.code == NSMigrationError) || (persistentStoreError.code == NSMigrationMissingMappingModelError));

	if ([persistentStoreError.domain isEqualToString:NSCocoaErrorDomain] && isMigrationError && [storeType isEqualToString:NSSQLiteStoreType]) {
		NSError *removeSQLiteFilesError = nil;
		if ([self removeSQLiteFilesAtStoreURL:self.storeURL error:&removeSQLiteFilesError]) {
			persistentStoreError = nil;
			persistentStore = [persistentStoreCoordinator addPersistentStoreWithType:storeType
																	   configuration:nil
																				 URL:storeURL
																			 options:options
																			   error:&persistentStoreError];
		} else {
			NSLog(@"Could not remove SQLite files\n%@", [removeSQLiteFilesError localizedDescription]);

			return nil;
		}
	}
	NSAssert(persistentStore != nil, @"Failed to create persistent store");

	return persistentStoreCoordinator;
}

- (BOOL)removeSQLiteFilesAtStoreURL:(NSURL *)storeURL error:(NSError * __autoreleasing *)error
{
	NSFileManager *fileManager = [NSFileManager defaultManager];
	NSURL *storeDirectory = [storeURL URLByDeletingLastPathComponent];
	NSDirectoryEnumerator *enumerator = [fileManager enumeratorAtURL:storeDirectory
										  includingPropertiesForKeys:nil
															 options:0
														errorHandler:nil];

	NSString *storeName = [storeURL.lastPathComponent stringByDeletingPathExtension];
	for (NSURL *url in enumerator) {
		if ([url.lastPathComponent hasPrefix:storeName] == NO) {
			continue;
		}

		NSError *fileManagerError = nil;
		if ([fileManager removeItemAtURL:url error:&fileManagerError] == NO) {
			if (error != NULL) {
				*error = fileManagerError;
			}
			return NO;
		}
	}
	return YES;
}

@end
